/*
 * getlistservice.h
 */

#ifndef _GETLISTSERVICE_H_
#define _GETLISTSERVICE_H_

#include <ssoa/service/servicestub.h>

namespace storageprovider
{
    /// Represents the service of retrieving a list of all images from the storage provider.
    class GetListService: public ssoa::ServiceStub
    {
        /// Just a shortcut.
        typedef unsigned char byte;

    public:
        /// Constructs a new instance of GetListService.
        ///
        /// @param host The remote host of the service provider.
        /// @param port The remote port on which the service is provided.
        GetListService(std::string host, std::string port) :
            ssoa::ServiceStub(ssoa::ServiceSignature("GetList(out buffer)"), host, port)
        {
        }

        /// Gets a string representing the status of the operation.
        const std::string& getStatus() const {
            return status;
        }

        /// Executes the service request, that is, retrieves the list of all images stored
        /// on the server.
        ///
        /// @param list A vector of strings that will contain the list. Items are added at
        ///        the end of this vector.
        bool invoke(std::vector<std::string>& list) {
            using namespace std;
            using namespace ssoa;

            unique_ptr<Response> response(ServiceStub::submit());
            unique_ptr<ServiceBufferArgument> arg(response->popArgument<ServiceBufferArgument>());
            vector<byte>& buffer = arg->getValue();

            char *ptr = (char*)buffer.data();
            char *end = ptr + buffer.size();
            do {
                list.push_back(std::string(ptr));
                ptr += list.back().size() + 1;
            } while (ptr < end);

            status = response->getStatus();
            return response->isSuccessful();
        }

    private:
        std::string status;
    };
}

#endif
