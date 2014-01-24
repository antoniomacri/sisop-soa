/*
 * getlistserviceimpl.cpp
 */

#include <getlistserviceimpl.h>
#include <storageservice.h>

#include <string>
#include <vector>

using std::string;
using std::vector;
using namespace ssoa;

namespace storageprovider
{
    Response * GetListServiceImpl::invoke()
    {
        vector<string> list = StorageService::getList();
        Logger::info("Built list of images (count: %1%).", list.size());

        vector<byte> buffer;
        for (auto it = list.begin(); it != list.end(); ++it) {
            std::copy(it->begin(), it->end(), std::back_inserter(buffer));
            buffer.push_back('\0');
        }

        Response * response = new Response(serviceSignature(), true, "OK");
        response->pushArgument(new ServiceBufferArgument(std::move(buffer)));
        return response;
    }
}
