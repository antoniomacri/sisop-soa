/*
 * getlistserviceimpl.cpp
 */

#include <getlistserviceimpl.h>

using namespace ssoa;

namespace storageprovider
{
    Response * GetListServiceImpl::invoke()
    {
        return NULL;
    }
}

static ssoa::ServiceSkeleton::installer<storageprovider::GetListServiceImpl> install;
