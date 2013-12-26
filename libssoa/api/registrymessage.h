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
    class RegistryMessage
    {
        typedef RegistryMessage * (*Factory)(const YAML::Node&);

    public:
        /// Constructs a new RegistryMessage from the given YAML string.
        /// Only the first YAML document is read, others are ignored.
        ///
        /// @param A string containing the YAML code.
        ///
        /// @return Never returns NULL (in case of error an exception is thrown).
        ///
        /// @throws YAML::Exception if @c yaml contains invalid YAML code.
        static RegistryMessage * fromYaml(const std::string & yaml);

        virtual std::string toYaml() const = 0;

        virtual ~RegistryMessage()
        {
        }

        static void install(const char * type, Factory creator);

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
        static std::map<std::string, Factory>& mappings();
    };
}

#endif
