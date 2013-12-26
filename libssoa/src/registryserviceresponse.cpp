/*
 * registryserviceresponse.cpp
 */

#include <registryserviceresponse.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryMessage * RegistryServiceResponse::fromYaml(const YAML::Node& node)
    {
        if (node["type"].to<string>() != type())
            throw std::logic_error("Message type mismatch");

        auto successful = node.FindValue("successful");
        if (!successful || successful->to<bool>()) {
            string host = node["host"].to<string>();
            string port = node["port"].to<string>();
            return new RegistryServiceResponse(host, port);
        }
        auto pstatus = node.FindValue("status");
        string status = pstatus ? pstatus->to<string>() : "";
        return new RegistryServiceResponse(status);
    }

    string RegistryServiceResponse::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << type();
        e << YAML::Key << "successful" << YAML::Value << successful;
        if (successful) {
            e << YAML::Key << "host" << YAML::Value << host;
            e << YAML::Key << "port" << YAML::Value << port;
        }
        if (!status.empty()) {
            e << YAML::Key << "status" << YAML::Value << status;
        }
        e << YAML::EndMap;
        return e.c_str();
    }
}

static ssoa::RegistryMessage::installer<ssoa::RegistryServiceResponse> install;
