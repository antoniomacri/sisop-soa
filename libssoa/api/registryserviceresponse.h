/*
 * registryserviceresponse.h
 */

#ifndef _REGISTRYSERVICERESPONSE_H_
#define _REGISTRYSERVICERESPONSE_H_

#include <registrymessage.h>
#include "registryservicerequest.h"

namespace ssoa
{
    class RegistryServiceResponse: public RegistryMessage
    {
    public:
        /// @param status It will be escaped
        RegistryServiceResponse(const RegistryServiceRequest * request, std::string host, std::string port) :
            RegistryMessage(TYPE_SERVICE_RESPONSE),
                request(request), host(std::move(host)), port(std::move(port)), successful(true)
        {
        }

        /// @param status It will be escaped
        RegistryServiceResponse(const RegistryServiceRequest * request, const std::string & status) :
            RegistryMessage(TYPE_SERVICE_RESPONSE),
                request(request), host(""), port(""), successful(false), status(escapeYaml(status))
        {
        }

        // TODO: it is really useful?
        const RegistryServiceRequest * getRequest() const {
            return request;
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
        const RegistryServiceRequest * request;
        const std::string host;
        const std::string port;
        const bool successful;
        const std::string status;
    };
}

#endif
