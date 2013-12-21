/*
 * enumhelper.cpp
 */

#include "enumhelper.h"

#include <cstring>

namespace ssoa
{
    const char * EnumHelper::serviceParamTypeStrings[] = {
        "invalid", "int", "double", "string", "buffer"
    };

    const char * EnumHelper::messageTypeStrings[] = {
        "invalid", "service-request", "service-response", "registration-request", "ack"
    };

    const char * EnumHelper::toString(ServiceSignature::Type type)
    {
        if (type >= ServiceSignature::TYPE_INVALID && type <= ServiceSignature::TYPE_LAST)
            return serviceParamTypeStrings[(int)type];
        return serviceParamTypeStrings[ServiceSignature::TYPE_INVALID];
    }

    const char * EnumHelper::toString(RegistryMessage::Type type)
    {
        if (type >= RegistryMessage::TYPE_INVALID && type <= RegistryMessage::TYPE_LAST)
            return messageTypeStrings[type];
        return messageTypeStrings[RegistryMessage::TYPE_INVALID];
    }

    template<>
    ServiceSignature::Type EnumHelper::fromString(const char name[])
    {
        for (int i = 0; i <= ServiceSignature::TYPE_LAST; i++) {
            if (std::strcmp(name, serviceParamTypeStrings[i]) == 0) {
                return (ServiceSignature::Type)i;
            }
        }
        return ServiceSignature::TYPE_INVALID;
    }

    template<>
    RegistryMessage::Type EnumHelper::fromString(const char name[])
    {
        for (int i = 0; i <= RegistryMessage::TYPE_LAST; i++) {
            if (std::strcmp(name, messageTypeStrings[i]) == 0) {
                return (RegistryMessage::Type)i;
            }
        }
        return RegistryMessage::TYPE_INVALID;
    }
}
