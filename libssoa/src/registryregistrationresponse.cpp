/*
 * registryregistrationresponse.cpp
 */

#include <registryregistrationresponse.h>
#include <enumhelper.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryRegistrationResponse * RegistryRegistrationResponse::fromYaml(const YAML::Node& node)
    {
        RegistryMessage::Type t = EnumHelper::fromString<RegistryMessage::Type>(node["type"].to<string>());
        if (t != TYPE_REGISTRATION_RESPONSE)
            throw std::logic_error("Message type mismatch");

        bool successful = node["successful"].to<bool>();
        string status = node["status"].to<string>();
        return new RegistryRegistrationResponse(successful, status);
    }

    string RegistryRegistrationResponse::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << EnumHelper::toString(TYPE_REGISTRATION_RESPONSE);
        e << YAML::Key << "successful" << YAML::Value << successful;
        if (!status.empty()) {
            e << YAML::Key << "status" << YAML::Value << status;
        }
        e << YAML::EndMap;
        return e.c_str();
    }
}
