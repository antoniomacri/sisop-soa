/*
 * registryregistrationrequest.h
 */

#ifndef _REGISTRYREGISTRATIONREQUEST_H_
#define _REGISTRYREGISTRATIONREQUEST_H_

#include <registrymessage.h>
#include <servicesignature.h>

namespace ssoa
{
    class RegistryRegistrationRequest: public RegistryMessage
    {
    public:
        RegistryRegistrationRequest(const ServiceSignature & service, std::string host, std::string port,
            bool unregister = false) :
            service(service), host(std::move(host)), port(std::move(port)), unregister(unregister)
        {
        }

        const ServiceSignature & getService() const {
            return service;
        }

        const std::string & getHost() const {
            return host;
        }

        const std::string & getPort() const {
            return port;
        }

        bool getUnregister() const {
            return unregister;
        }

        static const char * type() {
            return "registration-request";
        }

        static RegistryMessage * fromYaml(const YAML::Node& node);
        virtual std::string toYaml() const;

    private:
        ServiceSignature service;
        std::string host;
        std::string port;
        bool unregister;
    };
}

#endif
