/*
 * horizontalflipimageserviceimpl.h
 */

#ifndef _HORIZONTALFLIPIMAGESERVICEIMPL_H_
#define _HORIZONTALFLIPIMAGESERVICEIMPL_H_

#include <ssoa/service/servicesignature.h>
#include <ssoa/service/serviceskeleton.h>

namespace imagemanipulationprovider
{
    /// Implements the "HorizontalFlipImage" service.
    class HorizontalFlipImageServiceImpl: public ssoa::ServiceSkeleton
    {
        HorizontalFlipImageServiceImpl(arg_deque arguments) :
            ssoa::ServiceSkeleton(ssoa::ServiceSignature(serviceSignature()), std::move(arguments))
        {
        }

    public:
        /// Constructs a new instance of HorizontalFlipImageServiceImpl from the given arguments.
        static ssoa::ServiceSkeleton* create(arg_deque&& arguments) {
            return new HorizontalFlipImageServiceImpl(std::move(arguments));
        }

        /// Gets the signature of this type of service.
        static const char * serviceSignature() {
            return "HorizontalFlipImage(in buffer, out buffer)";
        }

        virtual ssoa::Response * invoke() {
            return NULL;
        }
    };
}

#endif
