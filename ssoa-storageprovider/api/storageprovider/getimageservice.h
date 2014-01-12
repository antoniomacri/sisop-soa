/*
 * getimageservice.h
 */

#ifndef _GETIMAGESERVICE_H_
#define _GETIMAGESERVICE_H_

#include <ssoa/service/servicestub.h>

namespace storageprovider
{
    /// Represents the service of retrieving an image from the storage provider.
    class GetImageService: public ssoa::ServiceStub
    {
        /// Just a shortcut.
        typedef unsigned char byte;

    public:
        /// Constructs a new instance of GetImageService.
        ///
        /// @param host The remote host of the service provider.
        /// @param port The remote port on which the service is provided.
        GetImageService(std::string host, std::string port) :
            ssoa::ServiceStub(ssoa::ServiceSignature(serviceSignature()), host, port)
        {
        }

        /// Gets the signature of this type of service.
        static const char * serviceSignature() {
            return "GetImage(in string, out buffer)";
        }

        /// Gets a string representing the status of the operation.
        const std::string& getStatus() const {
            return status;
        }

        /// Executes the service request, that is, retrieves the image with the given name
        /// from the server.
        ///
        /// @param name The name used to identify the image on the server.
        /// @param buffer The buffer that will contain the image data retrieved from the server.
        bool invoke(std::string name, std::vector<byte>& buffer) {
            using namespace std;
            using namespace ssoa;

            pushArgument(new ServiceStringArgument(name));

            unique_ptr<Response> response(ServiceStub::submit());
            if (response->isSuccessful()) {
                unique_ptr<ServiceBufferArgument> arg(response->popArgument<ServiceBufferArgument>());
                buffer = std::move(arg->getValue());
            }
            status = response->getStatus();
            return response->isSuccessful();
        }

    private:
        std::string status;
    };
}

#endif
