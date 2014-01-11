/*
 * getimageserviceimpl.h
 */

#ifndef _GETIMAGESERVICEIMPL_H_
#define _GETIMAGESERVICEIMPL_H_

#include <ssoa/service/servicesignature.h>
#include <ssoa/service/serviceskeleton.h>

namespace storageprovider
{
    /// Implements the "GetImage" service.
    class GetImageServiceImpl: public ssoa::ServiceSkeleton
    {
        GetImageServiceImpl(arg_deque arguments) :
            ssoa::ServiceSkeleton(ssoa::ServiceSignature(serviceSignature()), std::move(arguments))
        {
        }

    public:
        /// Constructs a new instance of GetImageServiceImpl from the given arguments.
        static ssoa::ServiceSkeleton* create(arg_deque&& arguments) {
            return new GetImageServiceImpl(std::move(arguments));
        }

        /// Gets the signature of this type of service.
        static const char * serviceSignature() {
            return "GetImage(in string, out buffer)";
        }

        /// Installs the creation method.
        static void install() {
            factory().install(serviceSignature(), create);
        }

        virtual ssoa::Response * invoke();
    };
}

#endif
