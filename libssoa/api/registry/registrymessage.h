/*
 * registrymessage.h
 */

#ifndef _REGISTRYMESSAGE_H_
#define _REGISTRYMESSAGE_H_

#include <map>
#include <string>

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
        /// Represents a method which generates a RegistryMessage from the given
        /// @c YAML::Node.
        /// This method must not return @c NULL. If the message cannot be constructed,
        /// an exception @c std::runtime_error must be thrown.
        typedef RegistryMessage * (*Factory)(const YAML::Node&);

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

        /// Installs a new deserialization handler for a specific message type.
        ///
        /// @param type A null-terminated string identifying the message type
        /// @param creator A function which handles the creation of the message.
        ///
        /// @see Factory
        static void install(const char * type, Factory creator);

        /// Simplifies the installation of a new deserialization handler.
        ///
        /// @tparam T The class to register for the deserialization process.
        ///
        /// The class @p T must meet two requirements:
        ///   1. must have a method @c type() returning a char array or a string
        ///      which contains the identifier of the message type
        ///   2. must have a static method @c fromYaml() with the following signature:
        ///      @code RegistryMessage * fromYaml(const YAML::Node& node); @endcode
        ///
        /// @see Factory for further information about the @c fromYaml() behavior.
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
                install(T::type(), T::fromYaml);
            }
        };

    protected:
        RegistryMessage()
        {
        }

    private:
        /// Contains mappings between a class identifier and its "named constructor"
        static std::map<std::string, Factory>& mappings();
    };
}

#endif
