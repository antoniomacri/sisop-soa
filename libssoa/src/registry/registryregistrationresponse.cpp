/*
 * registryregistrationresponse.cpp
 */

#include <registry/registryregistrationresponse.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryMessage * RegistryRegistrationResponse::fromYaml(const YAML::Node& node)
    {
        if (node["type"].to<string>() != messageType())
            throw std::logic_error("Message type mismatch");

        bool successful = node["successful"].to<bool>();
        string status = node.FindValue("status") ? node["status"].to<string>() : "";
        return new RegistryRegistrationResponse(successful, status);
    }

    string RegistryRegistrationResponse::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << messageType();
        e << YAML::Key << "successful" << YAML::Value << successful;
        if (!status.empty()) {
            e << YAML::Key << "status" << YAML::Value << status;
        }
        e << YAML::EndMap;
        return e.c_str();
    }
}

static ssoa::RegistryMessage::installer<ssoa::RegistryRegistrationResponse> install;
