/*
 * serviceskeleton.h
 */

#ifndef _SERVICESKELETON_H_
#define _SERVICESKELETON_H_

#include <service/service.h>
#include <service/response.h>
#include <factorybase.h>
#include <logger.h>

#include <boost/asio/ip/tcp.hpp>

namespace ssoa
{
    /// Represents a service from the server perspective.
    class ServiceSkeleton: public Service
    {
    public:
        /// Accepts a service request from a socket and processes it.
        static void start(std::unique_ptr<boost::asio::ip::tcp::socket> socket);

        /// Executes the service.
        virtual Response * invoke() = 0;

        /// Virtual destructor.
        virtual ~ServiceSkeleton() {
        }

    public:
        /// Just a shortcut.
        typedef FactoryBase<ServiceSkeleton, arg_deque&&> Factory;

        /// Gets an identifier of the ServiceSkeleton hierarchy (used by FactoryBase).
        static const char * hierarchyName() {
            return stringify(ServiceSkeleton);
        }

        /// Simplifies the installation of a new deserialization handler.
        ///
        /// @tparam T The class to register for the deserialization process.
        ///
        /// The class @p T must meet two requirements:
        ///   1. must have a method @c serviceSignature() returning a char array or a string
        ///      which contains the signature of the service (used as identifier of the class)
        ///   2. must have a static method @c create() with the following signature:
        ///      @code ServiceSkeleton* create(arg_deque&& args); @endcode
        ///
        /// @see Factory::CreatorMethod
        ///      for further information about the behavior of @c create().
        ///
        /// @par Example
        /// To install a new deserialization handler, it is sufficient to add a line like
        /// the following at the end of the implementation file (.cpp) of the derived class:
        /// @code
        ///   static ssoa::ServiceSkeleton::installer<ServiceSkeletonDerivedClass> install;
        /// @endcode
        template<typename T>
        struct installer
        {
            installer() {
                Factory::install(T::serviceSignature(), T::create);
                Logger::info() << hierarchyName()
                               << ": Installed service \"" << T::serviceSignature() << "\"."
                               << std::endl;
            }
        };

    protected:
        /// Constructs a new instance of ServiceSkeleton from the given service signature
        /// and arguments.
        ServiceSkeleton(ServiceSignature signature, arg_deque arguments) :
            Service(std::move(signature), std::move(arguments))
        {
        }

    private:
        friend class ServiceSkeletonSerializationHelper;
    };
}

#endif
