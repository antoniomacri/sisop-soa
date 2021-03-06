/*
 * storeimageserviceimpl.h
 */

#ifndef _STOREIMAGESERVICEIMPL_H_
#define _STOREIMAGESERVICEIMPL_H_

#include <ssoa/service/servicesignature.h>
#include <ssoa/service/serviceskeleton.h>

namespace storageprovider
{
    /// Implements the "StoreImage" service.
    class StoreImageServiceImpl: public ssoa::ServiceSkeleton
    {
        StoreImageServiceImpl(arg_deque arguments) :
            ssoa::ServiceSkeleton(ssoa::ServiceSignature(serviceSignature()), std::move(arguments))
        {
        }

    public:
        /// Constructs a new instance of StoreImageServiceImpl from the given arguments.
        static ssoa::ServiceSkeleton* create(arg_deque&& arguments) {
            return new StoreImageServiceImpl(std::move(arguments));
        }

        /// Gets the signature of this type of service.
        static const char * serviceSignature() {
            return "StoreImage(in string, in buffer)";
        }

        /// Installs the creation method.
        static void install() {
            factory().install(serviceSignature(), create);
        }

        virtual ssoa::Response * invoke();
    };
}

#endif
