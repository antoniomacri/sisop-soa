/*
 * registryservicerequest.h
 */

#ifndef _REGISTRYSERVICEREQUEST_H_
#define _REGISTRYSERVICEREQUEST_H_

#include <registrymessage.h>
#include <servicesignature.h>

namespace ssoa
{
    class RegistryServiceRequest: public RegistryMessage
    {
    public:
        RegistryServiceRequest(const ServiceSignature & service) :
            RegistryMessage(TYPE_SERVICE_REQUEST), service(service)
        {
        }

        const ServiceSignature & getService() const {
            return service;
        }

        static RegistryServiceRequest * fromYaml(const YAML::Node& node);
        virtual std::string toYaml() const;

    private:
        const ServiceSignature service;
    };
}

#endif
