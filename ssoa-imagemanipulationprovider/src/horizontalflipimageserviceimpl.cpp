/*
 * horizontalflipimageserviceimpl.cpp
 */

#include <horizontalflipimageserviceimpl.h>

using namespace ssoa;

namespace imagemanipulationprovider
{
    Response * HorizontalFlipImageServiceImpl::invoke()
    {
        return NULL;
    }
}

static ssoa::ServiceSkeleton::installer<imagemanipulationprovider::HorizontalFlipImageServiceImpl> install;
