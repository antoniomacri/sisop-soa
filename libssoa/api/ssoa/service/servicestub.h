/*
 * servicestub.h
 */

#ifndef _SERVICESTUB_H_
#define _SERVICESTUB_H_

#include <ssoa/service/service.h>
#include <ssoa/service/response.h>

#include <string>

namespace ssoa
{
    /// Represents a remote service from the client perspective.
    class ServiceStub: public Service
    {
    public:
        /// Constructs a new instance of ServiceStub.
        ///
        /// @param signature The signature of the remote service.
        /// @param host The remote address of the service provider.
        /// @param port The remote port on which the service is provided.
        ServiceStub(ServiceSignature signature, std::string host, std::string port) :
            Service(std::move(signature)), host(std::move(host)), port(std::move(port))
        {
        }

        /// Gets the remote address of the service provider.
        const std::string & getHost() const {
            return host;
        }

        /// Gets the remote port on which the service is provided.
        const std::string & getPort() const {
            return port;
        }

        /// Submits a service request and waits for the corresponding response.
        ///
        /// @returns The Response received from the remote server.
        Response * submit() const;

    private:
        std::string host;
        std::string port;
    };
}

#endif
