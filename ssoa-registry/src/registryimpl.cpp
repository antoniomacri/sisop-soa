/*
 * registryimpl.cpp
 */

#include "registryimpl.h"

#include <algorithm>

using std::string;

namespace ssoa
{
    RegistryImpl::RegistryImpl()
    {
    }

    bool RegistryImpl::registerService(ServiceSignature service, std::string host, std::string port)
    {
        if (service.isValid()) {
            std::lock_guard<std::mutex> lock(mutex);
            auto& list = services[service];
            auto pos = std::find(list.begin(), list.end(), std::make_pair(host, port));
            if (pos == list.end()) {
                list.emplace_front(host, port);
                return true;
            }
        }
        return false;
    }

    bool RegistryImpl::deregisterService(ServiceSignature service, std::string host, std::string port)
    {
        if (service == ServiceSignature::any) {
            return deregisterServer(host, port) > 0;
        }
        if (service.isValid()) {
            std::lock_guard<std::mutex> lock(mutex);
            auto pos = services.find(service);
            if (pos != services.end()) {
                auto& list = pos->second;
                auto posl = std::find(list.begin(), list.end(), std::make_pair(host, port));
                if (posl != list.end()) {
                    list.erase(posl);
                    return true;
                }
            }
        }
        return false;
    }

    int RegistryImpl::deregisterServer(std::string host, std::string port)
    {
        std::lock_guard<std::mutex> lock(mutex);
        int count = 0;
        auto pair = std::make_pair(host, port);
        for (auto it = services.begin(); it != services.end(); ++it) {
            auto& list = it->second;
            auto pos = std::find(list.begin(), list.end(), pair);
            if (pos != list.end()) {
                list.erase(pos);
                count++;
            }
        }
        return count;
    }

    bool RegistryImpl::lookupService(const ServiceSignature& service, std::string& host, std::string& port)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto pos = services.find(service);
        if (pos != services.end()) {
            auto& list = pos->second;
            auto& front = list.front();
            host = front.first;
            port = front.second;
            auto tail = front; // copy
            list.pop_front();
            list.push_back(tail);
            return true;
        }
        return false;
    }
}
