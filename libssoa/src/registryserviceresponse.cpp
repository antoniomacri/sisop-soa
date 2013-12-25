/*
 * registryserviceresponse.cpp
 */

#include <registryserviceresponse.h>
#include <enumhelper.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryServiceResponse * RegistryServiceResponse::fromYaml(const YAML::Node& node)
    {
        RegistryMessage::Type t = EnumHelper::fromString<RegistryMessage::Type>(node["type"].to<string>());
        if (t != TYPE_SERVICE_RESPONSE)
            throw std::logic_error("Message type mismatch");

        auto successful = node.FindValue("successful");
        if (!successful || successful->to<bool>()) {
            string host = node["host"].to<string>();
            string port = node["port"].to<string>();
            return new RegistryServiceResponse(NULL, host, port);
        }
        auto pstatus = node.FindValue("status");
        string status = pstatus ? pstatus->to<string>() : "";
        return new RegistryServiceResponse(NULL, status);
    }

    string RegistryServiceResponse::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << EnumHelper::toString(TYPE_SERVICE_RESPONSE);
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
