/*
 * storeimageserviceimpl.cpp
 */

#include <storeimageserviceimpl.h>

using namespace ssoa;

namespace storageprovider
{
    Response * StoreImageServiceImpl::invoke()
    {
        return NULL;
    }
}

static ssoa::ServiceSkeleton::installer<storageprovider::StoreImageServiceImpl> install;
