/*
 * servicesignature.h
 */

#ifndef _SERVICESIGNATURE_H_
#define _SERVICESIGNATURE_H_

#include <string>
#include <vector>

namespace ssoa
{
    class ServiceSignature
    {
    private:
        static std::string sigPattern;
        static std::string argPattern;

    public:
        static ServiceSignature any;

        enum Type
        {
            TYPE_INVALID = 0,
            TYPE_INT = 1,
            TYPE_DOUBLE = 2,
            TYPE_STRING = 3,
            TYPE_BUFFER = 4,
            TYPE_LAST = TYPE_BUFFER
        };

        ServiceSignature(const std::string & signature);
        ServiceSignature(const std::string & name, Type inputParams[], Type outputParams[]);

        const std::string & getName() const {
            return name;
        }

        const std::vector<Type> & getInputParams() const {
            return inputParams;
        }

        const std::vector<Type> & getOutputParams() const {
            return outputParams;
        }

        bool isValid() const {
            return is_valid;
        }

        bool operator==(const ServiceSignature & that) const {
            return this->signature == that.signature;
        }

        bool operator<(const ServiceSignature & that) const {
            return this->signature < that.signature;
        }

        operator std::string() const {
            return signature;
        }

    private:
        std::string name;
        std::string signature;
        std::vector<Type> inputParams;
        std::vector<Type> outputParams;
        bool is_valid;

        ServiceSignature() :
            name("*"), signature("*"), is_valid(false)
        {
        }
    };
}

#endif
