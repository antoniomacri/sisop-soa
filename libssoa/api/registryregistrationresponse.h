/*
 * registryregistrationresponse.h
 */

#ifndef _REGISTRYREGISTRATIONRESPONSE_H_
#define _REGISTRYREGISTRATIONRESPONSE_H_

#include <registrymessage.h>

namespace ssoa
{
    class RegistryRegistrationResponse: public RegistryMessage
    {
    public:
        RegistryRegistrationResponse(bool successful = true, std::string status = "") :
            RegistryMessage(TYPE_REGISTRATION_RESPONSE), successful(successful), status(std::move(status))
        {
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
        const bool successful;
        const std::string status;
    };
}

#endif
