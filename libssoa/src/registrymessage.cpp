/*
 * registrymessage.cpp
 */

#include <registrymessage.h>
#include <registryerrormessage.h>
#include <registryregistrationrequest.h>
#include <registryregistrationresponse.h>
#include <registryservicerequest.h>
#include <registryserviceresponse.h>
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
                return RegistryErrorMessage::fromYaml(node);
            case RegistryMessage::TYPE_INVALID:
                throw std::runtime_error("Invalid message type (\"" + type + "\").");
                break;
        }
        // Should never happen
        throw std::logic_error("Unrecognized message type (\"" + type + "\").");
    }
}
