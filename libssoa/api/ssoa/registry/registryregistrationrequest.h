/*
 * registryregistrationrequest.h
 */

#ifndef _REGISTRYREGISTRATIONREQUEST_H_
#define _REGISTRYREGISTRATIONREQUEST_H_

#include <ssoa/registry/registrymessage.h>
#include <ssoa/service/servicesignature.h>

namespace ssoa
{
    /// Represents a registration request issued to the registry.
    class RegistryRegistrationRequest: public RegistryMessage
    {
    public:
        /// Constructs a RegistryRegistrationRequest.
        ///
        /// @param service The signature of the service being registered.
        /// @param host A string (host name or IP address) identifying the service provider.
        /// @param port A string identifying the port on which the service is provided.
        /// @param deregister A boolean value indicating whether to deregister an already registered
        ///        service.
        RegistryRegistrationRequest(
            const ServiceSignature& service, std::string host, std::string port, bool deregister = false) :
            service(service), host(std::move(host)), port(std::move(port)), deregister(deregister)
        {
        }

        /// Gets the signature of the service.
        const ServiceSignature & getService() const {
            return service;
        }

        /// Gets the service provider's host name.
        const std::string & getHost() const {
            return host;
        }

        /// Gets the port on which the service is provided.
        const std::string & getPort() const {
            return port;
        }

        /// Gets a value indicating whether the service has to be deregistered.
        bool getDeregister() const {
            return deregister;
        }

        /// Gets the identifier of this type of message (used for deserialization).
        static const char * messageType() {
            return "registration-request";
        }

        /// Creates a new instance deserializing it from the specified YAML node.
        static RegistryMessage * fromYaml(const YAML::Node& node);

        /// Installs the creation method.
        static void install() {
            factory().install(messageType(), fromYaml);
        }

        virtual std::string toYaml() const;

    private:
        ServiceSignature service;
        std::string host;
        std::string port;
        bool deregister;
    };
}

#endif
