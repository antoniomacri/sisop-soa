/*
 * registrymessage.cpp
 */

#include <registrymessage.h>

#include <sstream>
#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::map;
using std::string;
using std::istringstream;

namespace ssoa
{
    RegistryMessage * RegistryMessage::fromYaml(const string& yaml)
    {
        istringstream ss(yaml);
        YAML::Parser parser(ss);
        YAML::Node node;
        parser.GetNextDocument(node);

        string type = node["type"].to<string>();
        auto iter = mappings().find(type);
        if (iter == mappings().end()) {
            throw std::runtime_error("Unknown message type: \"" + type + "\".");
        }
        return (iter->second)(node);
    }

    void RegistryMessage::install(const char * type, Factory creator)
    {
        string s(type);
        auto iter = mappings().find(s);
        if (iter != mappings().end()) {
            throw std::logic_error("Duplicated initialization for identifier '" + s + "'.");
        }
        mappings()[s] = creator;
    }

    map<string, RegistryMessage::Factory>& RegistryMessage::mappings()
    {
        // Construct-on-first-use to avoid the static initialization order problem
        static map<string, Factory> * m = new map<string, Factory>();
        return *m;
    }
}
