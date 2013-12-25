/*
 * registryregistrationresponse.h
 */

#ifndef _REGISTRYREGISTRATIONRESPONSE_H_
#define _REGISTRYREGISTRATIONRESPONSE_H_

#include <registrymessage.h>
#include <registryregistrationrequest.h>

namespace ssoa
{
    class RegistryRegistrationResponse: public RegistryMessage
    {
    public:
        RegistryRegistrationResponse(
            const RegistryRegistrationRequest * request, bool successful = true, std::string status = "") :
            RegistryMessage(TYPE_REGISTRATION_RESPONSE),
                request(request), successful(successful), status(std::move(status))
        {
        }

        const RegistryRegistrationRequest * getRequest() const {
            // TODO: it is really useful?
            return request;
        }

        bool isSuccessful() const {
            return successful;
        }

        const std::string & getStatus() const {
            return status;
        }

        static RegistryRegistrationResponse * fromYaml(const YAML::Node& node);
        virtual std::string toYaml() const;

    private:
        const RegistryRegistrationRequest * request;
        const bool successful;
        const std::string status;
    };
}

#endif
