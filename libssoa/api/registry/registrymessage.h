/*
 * registrymessage.h
 */

#ifndef _REGISTRYMESSAGE_H_
#define _REGISTRYMESSAGE_H_

#include <factorybase.h>

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
        /// Just a shortcut.
        typedef FactoryBase<RegistryMessage, const YAML::Node&> Factory;

        /// Gets an identifier of the RegistryMessage hierarchy (used by FactoryBase).
        static const char * hierarchyName() {
            return stringify(RegistryMessage);
        }

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

        /// Simplifies the installation of a new deserialization handler.
        ///
        /// @tparam T The class to register for the deserialization process.
        ///
        /// The class @p T must meet two requirements:
        ///   1. must have a method @c messageType() returning a char array or a string
        ///      which contains the identifier of the message type
        ///   2. must have a static method @c fromYaml() with the following signature:
        ///      @code RegistryMessage * fromYaml(const YAML::Node& node); @endcode
        ///
        /// @see Factory::CreatorMethod
        ///      for further information about the @c fromYaml() behavior.
        ///
        /// @par Example
        /// To install a new deserialization handler, it is sufficient to add a line like
        /// the following at the end of the implementation file (.cpp) of the derived class:
        /// @code
        ///   static ssoa::RegistryMessage::installer<RegistryMessageDerivedClass> install;
        /// @endcode
        template<typename T>
        struct installer
        {
            installer() {
                Factory::install(T::messageType(), T::fromYaml);
            }
        };

    protected:
        RegistryMessage()
        {
        }
    };
}

#endif
