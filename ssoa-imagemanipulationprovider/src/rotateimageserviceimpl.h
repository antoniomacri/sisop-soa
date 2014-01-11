/*
 * rotateimageserviceimpl.h
 */

#ifndef _ROTATEIMAGESERVICEIMPL_H_
#define _ROTATEIMAGESERVICEIMPL_H_

#include <ssoa/service/servicesignature.h>
#include <ssoa/service/serviceskeleton.h>

namespace imagemanipulationprovider
{
    /// Implements the "RotateImage" service.
    class RotateImageServiceImpl: public ssoa::ServiceSkeleton
    {
        RotateImageServiceImpl(arg_deque arguments) :
            ssoa::ServiceSkeleton(ssoa::ServiceSignature(serviceSignature()), std::move(arguments))
        {
        }

    public:
        /// Constructs a new instance of RotateImageServiceImpl from the given arguments.
        static ssoa::ServiceSkeleton* create(arg_deque&& arguments) {
            return new RotateImageServiceImpl(std::move(arguments));
        }

        /// Gets the signature of this type of service.
        static const char * serviceSignature() {
            return "RotateImage(in int, in buffer, out buffer)";
        }

        virtual ssoa::Response * invoke();
    };
}

#endif
