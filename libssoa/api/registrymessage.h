/*
 * registrymessage.h
 */

#ifndef _REGISTRYMESSAGE_H_
#define _REGISTRYMESSAGE_H_

#include <string>

#include <boost/algorithm/string/replace.hpp>

// Forward-declare YAML::Node
namespace YAML
{
    class Node;
}

namespace ssoa
{
    class RegistryMessage
    {
    public:
        enum Type
        {
            TYPE_INVALID = 0,
            TYPE_SERVICE_REQUEST,
            TYPE_SERVICE_RESPONSE,
            TYPE_REGISTRATION_REQUEST,
            TYPE_REGISTRATION_RESPONSE,
            TYPE_ERROR,
            TYPE_LAST = TYPE_ERROR
        };

        Type getType() {
            return type;
        }

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

    protected:
        RegistryMessage(Type type) :
            type(type)
        {
        }

        // TODO: improve escape/unescape
        static std::string escapeYaml(std::string text) {
            if (text.find_first_of(":'\"{}[]") == std::string::npos)
                return text;
            return "'" + boost::algorithm::replace_all_copy(text, "'", "\\'") + "'";
        }
        static std::string unescapeYaml(std::string text) {
            if (text.size() > 0 && text[0] == '\'' && text[text.size() - 1] == '\'')
                return boost::algorithm::replace_all_copy(text.substr(0, text.size() - 2), "\\'", "'");
            return text;
        }

    private:
        Type type;
    };
}

#endif
