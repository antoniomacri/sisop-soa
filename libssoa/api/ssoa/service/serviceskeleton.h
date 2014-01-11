/*
 * serviceskeleton.h
 */

#ifndef _SERVICESKELETON_H_
#define _SERVICESKELETON_H_

#include <ssoa/service/service.h>
#include <ssoa/service/response.h>
#include <ssoa/factorybase.h>
#include <ssoa/logger.h>

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

    protected:
        /// Constructs a new instance of ServiceSkeleton from the given service signature
        /// and arguments.
        ServiceSkeleton(ServiceSignature signature, arg_deque arguments) :
            Service(std::move(signature), std::move(arguments))
        {
        }

        /// Just a shortcut.
        typedef FactoryBase<ServiceSkeleton, arg_deque&&> Factory;

        /// Returns the factory object.
        static Factory& factory() {
            static Factory f(stringify(ServiceSkeleton));
            return f;
        }

    private:
        friend class ServiceSkeletonSerializationHelper;
    };
}

#endif
