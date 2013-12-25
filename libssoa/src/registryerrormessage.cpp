/*
 * registryerrormessage.cpp
 */

#include <registryerrormessage.h>
#include <enumhelper.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryErrorMessage * RegistryErrorMessage::fromYaml(const YAML::Node& node)
    {
        RegistryMessage::Type t = EnumHelper::fromString<RegistryMessage::Type>(node["type"].to<string>());
        if (t != TYPE_ERROR)
            throw std::logic_error("Message type mismatch");

        string status = unescapeYaml(node["status"].to<string>());
        return new RegistryErrorMessage(status);
    }

    string RegistryErrorMessage::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << EnumHelper::toString(TYPE_ERROR);
        e << YAML::Key << "status" << YAML::Value << escapeYaml(status);
        e << YAML::EndMap;
        return e.c_str();
    }
}
