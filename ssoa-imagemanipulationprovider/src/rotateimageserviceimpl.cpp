/*
 * rotateimageserviceimpl.cpp
 */

#include <rotateimageserviceimpl.h>

using namespace ssoa;

namespace imagemanipulationprovider
{
    Response * RotateImageServiceImpl::invoke()
    {
        return NULL;
    }
}

static ssoa::ServiceSkeleton::installer<imagemanipulationprovider::RotateImageServiceImpl> install;
