/*
 * enumhelper.h
 */

#ifndef _ENUMHELPER_H_
#define _ENUMHELPER_H_

#include <servicesignature.h>
#include <registrymessage.h>

#include <string>

namespace ssoa
{
    class EnumHelper
    {
    public:
        static const char * serviceParamTypeStrings[];
        static const char * messageTypeStrings[];

        template<typename T>
        static T fromString(const char name[]);

        template<typename T>
        static T fromString(const std::string & s)
        {
            return fromString<T>(s.c_str());
        }

        static const char * toString(ServiceSignature::Type type);
        static const char * toString(RegistryMessage::Type type);
    };
}

#endif
