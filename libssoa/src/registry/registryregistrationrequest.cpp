/*
 * registryregistrationrequest.cpp
 */

#include <registry/registryregistrationrequest.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryMessage * RegistryRegistrationRequest::fromYaml(const YAML::Node& node)
    {
        if (node["type"].to<string>() != type())
            throw std::logic_error("Message type mismatch");

        string service = node["service"].to<string>();
        string host = node["host"].to<string>();
        string port = node["port"].to<string>();
        bool unregister = node["unregister"].to<bool>();
        return new RegistryRegistrationRequest(service, host, port, unregister);
    }

    string RegistryRegistrationRequest::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << type();
        e << YAML::Key << "service" << YAML::Value << service;
        e << YAML::Key << "host" << YAML::Value << host;
        e << YAML::Key << "port" << YAML::Value << port;
        if (unregister) {
            e << YAML::Key << "unregister" << YAML::Value << unregister;
        }
        e << YAML::EndMap;
        return e.c_str();
    }
}

static ssoa::RegistryMessage::installer<ssoa::RegistryRegistrationRequest> install;
