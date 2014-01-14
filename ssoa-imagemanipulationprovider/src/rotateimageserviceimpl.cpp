/*
 * rotateimageserviceimpl.cpp
 */

#include <rotateimageserviceimpl.h>
#include <imagehelper.h>

#include <vector>

using std::vector;
using namespace ssoa;

namespace imagemanipulationprovider
{
    Response * RotateImageServiceImpl::invoke()
    {
        std::unique_ptr<ServiceIntArgument> degrees(popArgument<ServiceIntArgument>());
        std::unique_ptr<ServiceBufferArgument> inputBuffer(popArgument<ServiceBufferArgument>());

        vector<byte> buffer;
        ImageHelper::rotate(inputBuffer->getValue(), buffer, (float)degrees->getValue());

        Response * response = new Response(serviceSignature(), true, "OK");
        response->pushArgument(new ServiceBufferArgument(std::move(buffer)));
        return response;
    }
}
