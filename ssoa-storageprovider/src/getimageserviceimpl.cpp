/*
 * getimageserviceimpl.cpp
 */

#include <getimageserviceimpl.h>

using namespace ssoa;

namespace storageprovider
{
    Response * GetImageServiceImpl::invoke()
    {
        return NULL;
    }
}

static ssoa::ServiceSkeleton::installer<storageprovider::GetImageServiceImpl> install;
