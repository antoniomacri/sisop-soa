/*
 * getlistserviceimpl.h
 */

#ifndef _GETLISTSERVICEIMPL_H_
#define _GETLISTSERVICEIMPL_H_

#include <ssoa/service/servicesignature.h>
#include <ssoa/service/serviceskeleton.h>

namespace storageprovider
{
    /// Implements the "GetList" service.
    class GetListServiceImpl: public ssoa::ServiceSkeleton
    {
        GetListServiceImpl(arg_deque arguments) :
            ssoa::ServiceSkeleton(ssoa::ServiceSignature(serviceSignature()), std::move(arguments))
        {
        }

    public:
        /// Constructs a new instance of GetListServiceImpl from the given arguments.
        static ssoa::ServiceSkeleton* create(arg_deque&& arguments) {
            return new GetListServiceImpl(std::move(arguments));
        }

        /// Gets the signature of this type of service.
        static const char * serviceSignature() {
            return "GetList(out buffer)";
        }

        /// Installs the creation method.
        static void install() {
            factory().install(serviceSignature(), create);
        }

        virtual ssoa::Response * invoke();
    };
}

#endif
