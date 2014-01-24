/*
 * registryimpl.h
 */

#ifndef _REGISTRYIMPL_H_
#define _REGISTRYIMPL_H_

#include <ssoa/service/servicesignature.h>
#include <ssoa/registry/iregistry.h>

#include <list>
#include <map>
#include <string>

#include <boost/thread/shared_mutex.hpp>

namespace ssoa
{
    /// Implements a service registry.
    class RegistryImpl: public IRegistry
    {
    public:
        virtual bool registerService(const ServiceSignature& signature, std::string host, std::string port);

        virtual bool deregisterService(const ServiceSignature& signature, std::string host, std::string port);

        virtual int deregisterServer(std::string host, std::string port);

        virtual bool lookupService(const ServiceSignature& signature, std::string& host, std::string& port);

    private:
        struct service_data {
            ServiceSignature signature;
            std::vector<std::pair<std::string, std::string>> providers;
            int lastUsed;
        };

        typedef std::map<const std::string, service_data> service_data_map;

        // Each service is uniquely identified by its name (parameters must match).
        service_data_map services;

        /// A mutex to protect integrity of the registry data structures.
        boost::shared_mutex mutex;
    };
}

#endif
