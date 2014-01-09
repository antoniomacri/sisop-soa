/*
 * service.h
 */

#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <ssoa/service/serviceargument.h>
#include <ssoa/service/servicesignature.h>

#include <deque>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace ssoa
{
    class Service
    {
    public:
        Service(ServiceSignature signature) :
            signature(std::move(signature))
        {
        }

        /// Gets the name of the requested service.
        const std::string & getName() const {
            return signature.getName();
        }

        /// Gets the signature of the requested service.
        const ServiceSignature & getSignature() const {
            return signature;
        }

        template<class ServArg>
        void pushArgument(const ServArg *arg)
        {
            if (signature.getOutputParams()[arguments.size()] != ServArg::type()) {
                std::string expected = signature.getOutputParams()[arguments.size()];
                throw std::logic_error("Invalid argument (must be '" + expected + "').");
            }
            arguments.push_back(std::unique_ptr<const ServiceArgument>(arg));
        }

        template<class ServArg>
        const ServArg* popArgument() {
            ServiceArgument *a = arguments.front();
            ServArg *result = dynamic_cast<ServArg*>(a);
            if (result == NULL) {
                std::string expected = signature.getOutputParams()[0];
                throw std::logic_error("Invalid argument (must be '" + expected + "').");
            }
            arguments.pop_front();
            return result;
        }

    protected:
        /// Just a shortcut.
        typedef std::deque<std::unique_ptr<const ServiceArgument>> arg_deque;

        std::vector<boost::asio::const_buffer> getConstBuffers() const;

        Service(ServiceSignature signature, arg_deque arguments) :
            signature(std::move(signature)), arguments(std::move(arguments))
        {
        }

    private:
        ServiceSignature signature;
        arg_deque arguments;
    };
}

#endif
