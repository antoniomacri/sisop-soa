/*
 * storeimageservice.h
 */

#ifndef _STOREIMAGESERVICE_H_
#define _STOREIMAGESERVICE_H_

#include <ssoa/service/servicestub.h>

namespace storageprovider
{
    /// Represents the service of sending an image to the storage provider.
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

        /// Gets a string representing the status of the operation.
        const std::string& getStatus() const {
            return status;
        }

        /// Executes the service request, that is, sends an image with the given name
        /// to the server.
        ///
        /// @param name The name used to identify the image on the server.
        /// @param buffer The buffer containing the image data.
        bool invoke(std::string name, std::vector<byte> buffer) {
            using namespace std;
            using namespace ssoa;

            pushArgument(new ServiceStringArgument(name));
            pushArgument(new ServiceBufferArgument(buffer));

            unique_ptr<Response> response(ServiceStub::submit());
            status = response->getStatus();
            return response->isSuccessful();
        }

    private:
        std::string status;
    };
}

#endif
