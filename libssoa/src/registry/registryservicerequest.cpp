/*
 * registryservicerequest.cpp
 */

#include <ssoa/registry/registryservicerequest.h>

#include <sstream>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryMessage * RegistryServiceRequest::fromYaml(const YAML::Node& node)
    {
        if (node["type"].to<string>() != messageType())
            throw std::logic_error("Message type mismatch");

        string service = node["service"].to<string>();
        return new RegistryServiceRequest(service);
    }

    string RegistryServiceRequest::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << messageType();
        e << YAML::Key << "service" << YAML::Value << service;
        e << YAML::EndMap;
        return e.c_str();
    }
}
