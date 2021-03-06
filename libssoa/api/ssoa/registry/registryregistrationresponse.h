/*
 * registryregistrationresponse.h
 */

#ifndef _REGISTRYREGISTRATIONRESPONSE_H_
#define _REGISTRYREGISTRATIONRESPONSE_H_

#include <ssoa/registry/registrymessage.h>

namespace ssoa
{
    /// Represents a registration response produced by the registry.
    class RegistryRegistrationResponse: public RegistryMessage
    {
    public:
        /// Constructs an unsuccessful RegistryRegistrationResponse.
        ///
        /// @param status A string containing a status message.
        RegistryRegistrationResponse(std::string status) :
            successful(false), status(std::move(status))
        {
        }

        /// Constructs a RegistryRegistrationResponse.
        ///
        /// @param successful A boolean indicating whether the registration is successful.
        /// @param status A string containing a status message.
        RegistryRegistrationResponse(bool successful, std::string status) :
            successful(successful), status(std::move(status))
        {
        }

        /// Gets a boolean value indicating whether the registration is successful.
        bool isSuccessful() const {
            return successful;
        }

        /// Gets the status message.
        const std::string & getStatus() const {
            return status;
        }

        /// Gets the identifier of this type of message (used for deserialization).
        static const char * messageType() {
            return "registration-response";
        }

        /// Creates a new instance deserializing it from the specified YAML node.
        static RegistryMessage * fromYaml(const YAML::Node& node);

        /// Installs the creation method.
        static void install() {
            factory().install(messageType(), fromYaml);
        }

        virtual std::string toYaml() const;

    private:
        const bool successful;
        const std::string status;
    };
}

#endif
