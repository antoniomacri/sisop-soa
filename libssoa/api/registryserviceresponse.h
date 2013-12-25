/*
 * registryserviceresponse.h
 */

#ifndef _REGISTRYSERVICERESPONSE_H_
#define _REGISTRYSERVICERESPONSE_H_

#include <registrymessage.h>

namespace ssoa
{
    class RegistryServiceResponse: public RegistryMessage
    {
    public:
        RegistryServiceResponse(std::string host, std::string port) :
            RegistryMessage(TYPE_SERVICE_RESPONSE), host(std::move(host)), port(std::move(port)), successful(true)
        {
        }

        RegistryServiceResponse(std::string status) :
            RegistryMessage(TYPE_SERVICE_RESPONSE), host(""), port(""), successful(false), status(std::move(status))
        {
        }

        const std::string & getHost() const {
            return host;
        }

        const std::string & getPort() const {
            return port;
        }

        bool isSuccessful() const {
            return successful;
        }

        const std::string & getStatus() const {
            return status;
        }

        static RegistryServiceResponse * fromYaml(const YAML::Node& node);
        virtual std::string toYaml() const;

    private:
        const std::string host;
        const std::string port;
        const bool successful;
        const std::string status;
    };
}

#endif
