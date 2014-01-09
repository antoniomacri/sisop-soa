/*
 * registry.h
 */

#ifndef _REGISTRY_H_
#define _REGISTRY_H_

#include <ssoa/service/service.h>
#include <ssoa/registry/registrymessage.h>

#include <string>

using std::string;

namespace ssoa
{
    class Registry
    {
    public:
        // Not thread-safe
        static void initialize(string h, string p)
        {
            host = h;
            port = p;
        }

        static bool registerService(const ServiceSignature & signature, const string & host, const string & port);
        static bool deregisterService(const ServiceSignature & signature, const string & host, const string & port);
        static bool deregisterProvider(const string & host);

        static Service * getService(const ServiceSignature & signature);

        static RegistryMessage * submit(const RegistryMessage & signature);

    private:
        static string host;
        static string port;

        Registry()
        {
        }
    };
}

#endif
