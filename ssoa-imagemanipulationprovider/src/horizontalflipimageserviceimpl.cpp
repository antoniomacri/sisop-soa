/*
 * horizontalflipimageserviceimpl.cpp
 */

#include <horizontalflipimageserviceimpl.h>
#include <imagehelper.h>

#include <vector>

using std::vector;
using namespace ssoa;

namespace imagemanipulationprovider
{
    Response * HorizontalFlipImageServiceImpl::invoke()
    {
        std::unique_ptr<ServiceBufferArgument> inputBuffer(popArgument<ServiceBufferArgument>());

        vector<byte> buffer;
        ImageHelper::flipHorizontally(inputBuffer->getValue(), buffer);

        Response * response = new Response(serviceSignature(), true, "OK");
        response->pushArgument(new ServiceBufferArgument(std::move(buffer)));
        return response;
    }
}
