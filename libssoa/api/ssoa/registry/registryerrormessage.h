/*
 * registryerrormessage.h
 */

#ifndef _REGISTRYERRORMESSAGE_H_
#define _REGISTRYERRORMESSAGE_H_

#include <ssoa/registry/registrymessage.h>

namespace ssoa
{
    /// Represents an error condition.
    class RegistryErrorMessage: public RegistryMessage
    {
    public:
        /// Constructs a RegistryErrorMessage with the specified status message.
        RegistryErrorMessage(std::string status) :
            status(std::move(status))
        {
        }

        /// Gets the status message.
        const std::string & getStatus() const {
            return status;
        }

        /// Gets the identifier of this type of message (used for deserialization).
        static const char * messageType() {
            return "error";
        }

        /// Creates a new instance deserializing it from the specified YAML node.
        static RegistryMessage * fromYaml(const YAML::Node& node);

        /// Installs the creation method.
        static void install() {
            factory().install(messageType(), fromYaml);
        }

        virtual std::string toYaml() const;

    private:
        std::string status;
    };
}

#endif
