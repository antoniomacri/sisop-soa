/*
 * registrymessage.h
 */

#ifndef _REGISTRYMESSAGE_H_
#define _REGISTRYMESSAGE_H_

#include <ssoa/factorybase.h>

// Forward-declare YAML::Node
namespace YAML
{
    class Node;
}

namespace ssoa
{
    /// Represents a message exchanged with the registry.
    class RegistryMessage
    {
    public:
        /// Constructs a new RegistryMessage from the given YAML string.
        ///
        /// Only the first YAML document is read, others are ignored.
        ///
        /// @param yaml A string containing the YAML code.
        ///
        /// @return A RegistryMessage deserialized from the given YAML string.
        ///         The return value is never @c NULL.
        ///
        /// @throws YAML::Exception The argument @p yaml contains invalid YAML code.
        /// @throws std::runtime_error couldn't deserialize the object (e.g., the message
        ///         type is unknown).
        static RegistryMessage * fromYaml(const std::string & yaml);

        /// Builds a string containing the YAML representation of this message.
        ///
        /// @return A string containing the YAML code of this message.
        virtual std::string toYaml() const = 0;

        virtual ~RegistryMessage()
        {
        }

    protected:
        RegistryMessage()
        {
        }

        /// Just a shortcut.
        typedef FactoryBase<RegistryMessage, const YAML::Node&> Factory;

        /// Returns the factory object.
        static Factory& factory() {
            static Factory f(stringify(RegistryMessage));
            return f;
        }
    };
}

#endif
