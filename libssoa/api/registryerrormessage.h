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
        RegistryErrorMessage(std::string status) :
            status(std::move(status))
        {
        }

        const std::string & getStatus() const {
            return status;
        }

        static const char * type() {
            return "error";
        }

        static RegistryMessage * fromYaml(const YAML::Node& node);
        virtual std::string toYaml() const;

    private:
        std::string status;
    };
}

#endif
