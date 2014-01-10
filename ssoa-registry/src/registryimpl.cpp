/*
 * registryimpl.cpp
 */

#include "registryimpl.h"

#include <algorithm>
#include <vector>

using std::string;
using std::vector;

namespace ssoa
{
    bool RegistryImpl::registerService(const ServiceSignature& signature, string host, string port)
    {
        if (!signature.isValid()) {
            throw std::runtime_error("The specified signature is not valid.");
        }
        std::lock_guard<std::mutex> lock(mutex);
        auto& data = services[signature.getName()];
        if (data.signature == ServiceSignature::any) {
            data.signature = signature;
        }
        else if (data.signature != signature) {
            throw std::runtime_error("Is already registered a service with same name and different signature.");
        }
        auto& list = data.providers;
        std::pair<string, string> pair = std::make_pair(std::move(host), std::move(port));
        auto pos = std::find(list.begin(), list.end(), pair);
        if (pos == list.end()) {
            list.push_back(pair);
            return true;
        }
        return false;
    }

    bool RegistryImpl::deregisterService(const ServiceSignature& signature, string host, string port)
    {
        if (signature == ServiceSignature::any) {
            return deregisterServer(host, port) > 0;
        }
        std::lock_guard<std::mutex> lock(mutex);
        auto pos = services.find(signature.getName());
        if (pos != services.end()) {
            auto& data = pos->second;
            if (data.signature == signature) {
                auto& list = data.providers;
                std::pair<string, string> pair = std::make_pair(std::move(host), std::move(port));
                auto posl = std::find(list.begin(), list.end(), pair);
                if (posl != list.end()) {
                    list.erase(posl);
                    data.lastUsed = list.size() ? data.lastUsed % list.size() : 0;
                    if (list.empty()) {
                        services.erase(pos);
                    }
                    return true;
                }
            }
        }
        return false;
    }

    int RegistryImpl::deregisterServer(string host, string port)
    {
        std::lock_guard<std::mutex> lock(mutex);
        std::pair<string, string> pair = std::make_pair(std::move(host), std::move(port));
        int count = 0;
        vector<service_data_map::iterator> emptyEntries;
        for (auto it = services.begin(); it != services.end(); ++it) {
            auto& data = it->second;
            auto& list = data.providers;
            auto pos = std::find(list.begin(), list.end(), pair);
            if (pos != list.end()) {
                list.erase(pos);
                data.lastUsed = list.size() ? data.lastUsed % list.size() : 0;
                if (list.empty()) {
                    emptyEntries.push_back(it);
                }
                count++;
            }
        }
        // We cannot modify the map while iterating over it.
        for (size_t i = 0; i < emptyEntries.size(); i++) {
            services.erase(emptyEntries[i]);
        }
        return count;
    }

    bool RegistryImpl::lookupService(const ServiceSignature& signature, string& host, string& port)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto pos = services.find(signature.getName());
        if (pos != services.end()) {
            auto& data = pos->second;
            auto& list = data.providers;
            host = list[data.lastUsed].first;
            port = list[data.lastUsed].second;
            data.lastUsed = list.size() ? (data.lastUsed + 1) % list.size() : 0;
            return true;
        }
        return false;
    }
}
