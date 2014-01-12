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
    /// Base class used to represent a service in both the client (stub) and the server (skeleton).
    class Service
    {
    public:
        /// Constructs a new Service.
        ///
        /// @param signature The signature of the service.
        Service(ServiceSignature signature) :
            signature(std::move(signature)), pushed(0)
        {
        }

        /// Gets the name of the requested service.
        const std::string& getName() const {
            return signature.getName();
        }

        /// Gets the signature of the requested service.
        const ServiceSignature& getSignature() const {
            return signature;
        }

        /// Adds an argument to the list of input arguments.
        ///
        /// @tparam ServArg The actual type of the argument (derived of ServiceArgument).
        ///         The template is only used to check the value returned by its @c type() method.
        ///
        /// @param arg A pointer to a ServiceArgument object which has to be added.
        ///        The ownership of @c arg is transferred to this Service instance.
        ///
        /// @throws std::logic_error If all arguments have already been pushed, or if the given
        ///         ServiceArgument does not match the one stated in the signature.
        template<class ServArg>
        void pushArgument(ServArg *arg)
        {
            if (pushed == signature.getInputParams().size()) {
                throw std::logic_error("All arguments already pushed.");
            }
            std::string expected = signature.getInputParams()[pushed];
            if (expected != ServArg::type()) {
                throw std::logic_error("Invalid argument (must be '" + expected + "').");
            }
            arguments.emplace_back(arg);
            pushed++;
        }

        /// Gets an argument from the list of input arguments and removes it from the list.
        ///
        /// @tparam ServArg The actual type of the argument (derived of ServiceArgument).
        ///
        /// @return A pointer to the retrieved ServiceArgument object.
        ///         The ownership of @c arg is transferred to the caller.
        ///
        /// @throws std::logic_error If not all arguments have been pushed yet, or if all
        ///         arguments have already been popped, or if the specified ServiceArgument
        ///         does not match the one stated in the signature.
        template<class ServArg>
        ServArg* popArgument()
        {
            if (pushed != signature.getInputParams().size()) {
                throw std::logic_error("Not all arguments have been pushed.");
            }
            if (arguments.size() <= 0) {
                throw std::logic_error("All arguments already popped.");
            }

            ServiceArgument *a = arguments.front().get();
            ServArg *result = dynamic_cast<ServArg*>(a);
            if (result == NULL) {
                int index = signature.getInputParams().size() - arguments.size();
                std::string expected = signature.getInputParams()[index];
                throw std::logic_error("Invalid argument (must be '" + expected + "').");
            }
            arguments.front().release();
            arguments.pop_front();
            return result;
        }

    protected:
        /// Just a shortcut.
        typedef std::deque<std::unique_ptr<ServiceArgument>> arg_deque;

        /// Builds a ConstBufferSequence which can be used to serialize this Service.
        ///
        /// The Service keeps ownership of memory referred to by all buffers, which can
        /// can be invalidated by any non-const method invoked on this instance.
        std::vector<boost::asio::const_buffer> getConstBuffers() const;

        Service(ServiceSignature signature, arg_deque arguments) :
            signature(std::move(signature)), arguments(std::move(arguments)), pushed(0)
        {
        }

    private:
        ServiceSignature signature;
        arg_deque arguments;
        int pushed;
        mutable std::string header; // Temporarily keeps the header
    };
}

#endif
