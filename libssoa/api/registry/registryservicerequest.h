/*
 * registryservicerequest.h
 */

#ifndef _REGISTRYSERVICEREQUEST_H_
#define _REGISTRYSERVICEREQUEST_H_

#include <registry/registrymessage.h>
#include <service/servicesignature.h>

namespace ssoa
{
    /// Represents a service request issued to the registry.
    class RegistryServiceRequest: public RegistryMessage
    {
    public:
        /// Constructs a RegistryServiceRequest.
        ///
        /// @param service The signature of the requested service.
        RegistryServiceRequest(const ServiceSignature & service) :
            service(service)
        {
        }

        /// Gets the signature of the service.
        const ServiceSignature & getService() const {
            return service;
        }

        /// Gets the identifier of this type of message (used for deserialization).
        static const char * messageType() {
            return "service-request";
        }

        /// Creates a new instance deserializing it from the specified YAML node.
        static RegistryMessage * fromYaml(const YAML::Node& node);

        virtual std::string toYaml() const;

    private:
        const ServiceSignature service;
    };
}

#endif
