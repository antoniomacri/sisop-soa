/*
 * registryservicerequest.cpp
 */

#include <registryservicerequest.h>
#include <enumhelper.h>

#include <sstream>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryServiceRequest * RegistryServiceRequest::fromYaml(const YAML::Node& node)
    {
        RegistryMessage::Type t = EnumHelper::fromString<RegistryMessage::Type>(node["type"].to<string>());
        if (t != TYPE_SERVICE_REQUEST)
            throw std::logic_error("Message type mismatch");

        string service = node["service"].to<string>();
        return new RegistryServiceRequest(service);
    }

    string RegistryServiceRequest::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << EnumHelper::toString(TYPE_SERVICE_REQUEST);
        e << YAML::Key << "service" << YAML::Value << service;
        e << YAML::EndMap;
        return e.c_str();
    }
}
