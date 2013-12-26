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
            host(std::move(host)), port(std::move(port)), successful(true)
        {
        }

        RegistryServiceResponse(std::string status) :
            host(""), port(""), successful(false), status(std::move(status))
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

        static const char * type() {
            return "service-response";
        }

        static RegistryMessage * fromYaml(const YAML::Node& node);
        virtual std::string toYaml() const;

    private:
        const std::string host;
        const std::string port;
        const bool successful;
        const std::string status;
    };
}

#endif
