/*
 * registrymessage.cpp
 */

#include <ssoa/registry/registrymessage.h>

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
        return factory().create(type, node);
    }
}
