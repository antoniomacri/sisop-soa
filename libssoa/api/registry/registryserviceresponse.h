/*
 * registryserviceresponse.h
 */

#ifndef _REGISTRYSERVICERESPONSE_H_
#define _REGISTRYSERVICERESPONSE_H_

#include <registry/registrymessage.h>

namespace ssoa
{
    /// Represents a service response produced by the registry.
    class RegistryServiceResponse: public RegistryMessage
    {
    public:
        /// Constructs a successful RegistryServiceResponse.
        ///
        /// @param host A string (host name or IP address) identifying the service provider.
        /// @param port A string identifying the port on which the service is provided.
        RegistryServiceResponse(std::string host, std::string port) :
            host(std::move(host)), port(std::move(port)), successful(true)
        {
        }

        /// Constructs an unsuccessful RegistryServiceResponse.
        ///
        /// @param status A string containing a status message.
        RegistryServiceResponse(std::string status) :
            successful(false), status(std::move(status))
        {
        }

        /// Gets a string (host name or IP address) identifying the service provider.
        const std::string & getHost() const {
            return host;
        }

        /// Gets the port on which the service is provided.
        const std::string & getPort() const {
            return port;
        }

        /// Gets a boolean value indicating whether the request is successful.
        bool isSuccessful() const {
            return successful;
        }

        /// Gets the status message.
        const std::string & getStatus() const {
            return status;
        }

        /// Gets the identifier of this type of message (used for deserialization).
        static const char * type() {
            return "service-response";
        }

        /// Creates a new instance deserializing it from the specified YAML node.
        static RegistryMessage * fromYaml(const YAML::Node& node);

        virtual std::string toYaml() const;

    private:
        const std::string host;
        const std::string port;
        const bool successful;
        const std::string status;
    };
}

#endif
