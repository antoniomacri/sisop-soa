/*
 * registrymessage.cpp
 */

#include "registrymessage.h"
#include <enumhelper.h>

#include <sstream>
#include <stdexcept>

#include <yaml-cpp/yaml.h>

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
        switch (EnumHelper::fromString<RegistryMessage::Type>(type)) {
            case RegistryMessage::TYPE_REGISTRATION_REQUEST:
                return RegistryRegistrationRequest::fromYaml(node);
            case RegistryMessage::TYPE_REGISTRATION_RESPONSE:
                return RegistryRegistrationResponse::fromYaml(node);
            case RegistryMessage::TYPE_SERVICE_REQUEST:
                return RegistryServiceRequest::fromYaml(node);
            case RegistryMessage::TYPE_SERVICE_RESPONSE:
                return RegistryServiceResponse::fromYaml(node);
            case RegistryMessage::TYPE_ERROR:
                return RegistryServiceResponse::fromYaml(node);
            case RegistryMessage::TYPE_INVALID:
                throw std::runtime_error("Invalid message type (\"" + type + "\").");
                break;
        }
        // Should never happen
        throw std::logic_error("Unrecognized message type (\"" + type + "\").");
    }

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

    RegistryRegistrationRequest * RegistryRegistrationRequest::fromYaml(const YAML::Node& node)
    {
        RegistryMessage::Type t = EnumHelper::fromString<RegistryMessage::Type>(node["type"].to<string>());
        if (t != TYPE_REGISTRATION_REQUEST)
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
        e << YAML::Key << "type" << YAML::Value << EnumHelper::toString(TYPE_REGISTRATION_REQUEST);
        e << YAML::Key << "service" << YAML::Value << service;
        e << YAML::Key << "host" << YAML::Value << host;
        e << YAML::Key << "port" << YAML::Value << port;
        if (unregister) {
            e << YAML::Key << "unregister" << YAML::Value << unregister;
        }
        e << YAML::EndMap;
        return e.c_str();
    }

    RegistryRegistrationResponse * RegistryRegistrationResponse::fromYaml(const YAML::Node& node)
    {
        RegistryMessage::Type t = EnumHelper::fromString<RegistryMessage::Type>(node["type"].to<string>());
        if (t != TYPE_REGISTRATION_RESPONSE)
            throw std::logic_error("Message type mismatch");

        bool successful = node["successful"].to<bool>();
        string status = unescapeYaml(node["status"].to<string>());
        return new RegistryRegistrationResponse(NULL, successful, status);
    }

    string RegistryRegistrationResponse::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << EnumHelper::toString(TYPE_REGISTRATION_RESPONSE);
        e << YAML::Key << "successful" << YAML::Value << successful;
        // TODO: print request?
        if (!status.empty()) {
            e << YAML::Key << "status" << YAML::Value << escapeYaml(status);
        }
        e << YAML::EndMap;
        return e.c_str();
    }

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
        string status = pstatus ? escapeYaml(pstatus->to<string>()) : "";
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
            e << YAML::Key << "status" << YAML::Value << escapeYaml(status);
        }
        e << YAML::EndMap;
        return e.c_str();
    }
}
