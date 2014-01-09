/*
 * horizontalflipimageservice.h
 */

#ifndef _HORIZONTALFLIPIMAGESERVICE_H_
#define _HORIZONTALFLIPIMAGESERVICE_H_

#include <ssoa/service/servicestub.h>

namespace imagemanipulationprovider
{
    /// Represents a service which flips horizontally an image.
    class HorizontalFlipImageService: public ssoa::ServiceStub
    {
        /// Just a shortcut.
        typedef unsigned char byte;

    public:
        /// Constructs a new instance of HorizontalFlipImageService.
        ///
        /// @param host The remote host of the service provider.
        /// @param port The remote port on which the service is provided.
        HorizontalFlipImageService(std::string host, std::string port) :
            ssoa::ServiceStub(ssoa::ServiceSignature(serviceSignature()), host, port)
        {
        }

        /// Gets the signature of this type of service.
        static const char * serviceSignature() {
            return "HorizontalFlipImage(in buffer, out buffer)";
        }

        /// Gets a string representing the status of the operation.
        const std::string& getStatus() const {
            return status;
        }

        /// Executes the service request, that is, flips horizontally the given image.
        ///
        /// @param input The input buffer containing the image data.
        /// @param output The output buffer that will contain the flipped image.
        bool invoke(const std::vector<byte>& input, std::vector<byte>& output) {
            using namespace std;
            using namespace ssoa;

            pushArgument(new ServiceBufferArgument(input));

            unique_ptr<Response> response(ServiceStub::submit());
            unique_ptr<ServiceBufferArgument> arg(response->popArgument<ServiceBufferArgument>());
            output = std::move(arg->getValue());
            status = response->getStatus();
            return response->isSuccessful();
        }

    private:
        std::string status;
    };
}

#endif
