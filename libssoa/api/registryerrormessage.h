/*
 * registryerrormessage.h
 */

#ifndef _REGISTRYERRORMESSAGE_H_
#define _REGISTRYERRORMESSAGE_H_

#include <registrymessage.h>

namespace ssoa
{
    class RegistryErrorMessage: public RegistryMessage
    {
    public:
        RegistryErrorMessage(const std::string & status) :
            RegistryMessage(TYPE_ERROR), status(status)
        {
        }

        const std::string & getStatus() const {
            return status;
        }

        static RegistryErrorMessage * fromYaml(const YAML::Node& node);
        virtual std::string toYaml() const;

    private:
        std::string status;
    };
}

#endif
