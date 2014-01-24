/*
 * clienthandler.h
 */

#ifndef _CLIENTHANDLER_H_
#define _CLIENTHANDLER_H_

#include <ssoa/registry/iregistry.h>
#include <ssoa/registry/registryregistrationrequest.h>
#include <ssoa/registry/registryservicerequest.h>

#include <string>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/noncopyable.hpp>

namespace ssoa
{
    /// Represents an object which handles a single client.
    class ClientHandler: public std::enable_shared_from_this<ClientHandler>, private boost::noncopyable
    {
    public:
        /// Constructs a ClientHandler with the given io_service.
        explicit ClientHandler(boost::asio::io_service& io_service, IRegistry& registry);

        /// Gets the socket associated with the connection.
        boost::asio::ip::tcp::socket& getSocket();

        /// Starts the first asynchronous operation for the connection.
        void start();

    private:
        /// Handles completion of a read operation.
        void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred);

        /// Handles completion of a write operation.
        void handleWrite(const boost::system::error_code& e);

        /// Handles a request and produces a reply.
        std::string generateResponse(RegistryMessage *request);

        std::string generateRegistrationResponse(RegistryRegistrationRequest *request);
        std::string generateServiceResponse(RegistryServiceRequest *request);

        /// Socket for the connection.
        boost::asio::ip::tcp::socket socket;

        /// A reference to the actual registry implementation.
        IRegistry& registry;

        /// Buffer for incoming data.
        boost::asio::streambuf buffer;

        /// Contains the response text.
        std::string response;
    };
}

#endif
