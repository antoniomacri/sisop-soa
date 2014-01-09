/*
 * registryregistrationrequest.cpp
 */

#include <ssoa/registry/registryregistrationrequest.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryMessage * RegistryRegistrationRequest::fromYaml(const YAML::Node& node)
    {
        if (node["type"].to<string>() != messageType())
            throw std::logic_error("Message type mismatch");

        string service = node["service"].to<string>();
        string host = node["host"].to<string>();
        string port = node["port"].to<string>();
        bool deregister = node.FindValue("deregister") ? node["deregister"].to<bool>() : false;
        return new RegistryRegistrationRequest(service, host, port, deregister);
    }

    string RegistryRegistrationRequest::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << messageType();
        e << YAML::Key << "service" << YAML::Value << service;
        e << YAML::Key << "host" << YAML::Value << host;
        e << YAML::Key << "port" << YAML::Value << port;
        if (deregister) {
            e << YAML::Key << "deregister" << YAML::Value << deregister;
        }
        e << YAML::EndMap;
        return e.c_str();
    }
}

static ssoa::RegistryMessage::installer<ssoa::RegistryRegistrationRequest> install;
