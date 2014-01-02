/*
 * registryimpl.h
 */

#ifndef _REGISTRYIMPL_H_
#define _REGISTRYIMPL_H_

#include <service/servicesignature.h>

#include <list>
#include <map>
#include <mutex>
#include <string>

namespace ssoa
{
    class RegistryImpl
    {
    public:
        RegistryImpl();

        bool registerService(ServiceSignature service, std::string host, std::string port);

        bool unregisterService(ServiceSignature service, std::string host, std::string port);

        int unregisterServer(std::string host, std::string port);

        bool lookupService(const ServiceSignature& service, std::string& host, std::string& port);

    private:
        // Each service is uniquely identified by its name. Parameters must match
        std::map<ServiceSignature, std::list<std::pair<std::string, std::string> > > services;

        std::mutex mutex;
    };
}

#endif
