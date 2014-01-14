/*
 * storeimageserviceimpl.cpp
 */

#include <storeimageserviceimpl.h>
#include <storageservice.h>

using namespace ssoa;

namespace storageprovider
{
    Response * StoreImageServiceImpl::invoke()
    {
        std::unique_ptr<ServiceStringArgument> name(popArgument<ServiceStringArgument>());
        std::unique_ptr<ServiceBufferArgument> buffer(popArgument<ServiceBufferArgument>());

        StorageService::saveFile(name->getValue(), buffer->getValue());
        Logger::info() << "Stored image '" << name->getValue() << "'." << std::endl;

        return new Response(serviceSignature(), true, "OK");
    }
}
