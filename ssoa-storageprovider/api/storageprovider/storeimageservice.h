/*
 * storeimageservice.h
 */

#ifndef _STOREIMAGESERVICE_H_
#define _STOREIMAGESERVICE_H_

#include <ssoa/service/servicestub.h>

namespace storageprovider
{
    /// Represents an image-storage service from the client perspective.
    class StoreImageService: public ssoa::ServiceStub
    {
        /// Just a shortcut.
        typedef unsigned char byte;

    public:
        /// Constructs a new instance of StoreImageService.
        ///
        /// @param host The remote host of the service provider.
        /// @param port The remote port on which the service is provided.
        StoreImageService(std::string host, std::string port) :
            ssoa::ServiceStub(ssoa::ServiceSignature("StoreImage(in string, in buffer)"), host, port)
        {
        }

        /// Executes the service request, that is, sends an image with the given name
        /// to the server.
        ///
        /// @param name The name used to identify the image on the server.
        /// @param buffer The buffer containing the image data.
        bool invoke(std::string name, std::vector<byte> buffer) {
            pushArgument(new ssoa::ServiceStringArgument(name));
            pushArgument(new ssoa::ServiceBufferArgument(buffer));

            std::unique_ptr<ssoa::Response> response(ServiceStub::submit());
            return response->isSuccessful();
        }
    };
}

#endif
