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

        ServiceSignature(const std::string & signature);

        const std::string & getName() const {
            return name;
        }

        const std::vector<std::string> & getInputParams() const {
            return inputParams;
        }

        const std::vector<std::string> & getOutputParams() const {
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
        std::vector<std::string> inputParams;
        std::vector<std::string> outputParams;
        bool is_valid;

        ServiceSignature() :
            name("*"), signature("*"), is_valid(false)
        {
        }
    };
}

#endif
