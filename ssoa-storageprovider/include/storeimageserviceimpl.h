/*
 * storeimageserviceimpl.h
 */

#ifndef _STOREIMAGESERVICEIMPL_H_
#define _STOREIMAGESERVICEIMPL_H_

#include <service/servicesignature.h>
#include <service/serviceskeleton.h>

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

        virtual ssoa::Response * invoke() {
            return NULL;
        }
    };
}

#endif
