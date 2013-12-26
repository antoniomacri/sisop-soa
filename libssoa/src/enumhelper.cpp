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

    const char * EnumHelper::toString(ServiceSignature::Type type)
    {
        if (type >= ServiceSignature::TYPE_INVALID && type <= ServiceSignature::TYPE_LAST)
            return serviceParamTypeStrings[(int)type];
        return serviceParamTypeStrings[ServiceSignature::TYPE_INVALID];
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
}
