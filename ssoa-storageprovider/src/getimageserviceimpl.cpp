/*
 * getimageserviceimpl.cpp
 */

#include <getimageserviceimpl.h>
#include <storageservice.h>

#include <string>
#include <vector>

using std::string;
using std::vector;
using namespace ssoa;

namespace storageprovider
{
    Response * GetImageServiceImpl::invoke()
    {
        std::unique_ptr<ServiceStringArgument> name(popArgument<ServiceStringArgument>());

        vector<unsigned char> buffer;
        StorageService::loadFile(name->getValue(), buffer);

        Response * response = new Response(serviceSignature(), true, "OK");
        response->pushArgument(new ServiceBufferArgument(std::move(buffer)));
        return response;
    }
}
