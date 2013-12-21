/*
 * registrylistener.h
 */

#ifndef _REGISTRYLISTENER_H_
#define _REGISTRYLISTENER_H_

#include <string>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include "clienthandler.h"

namespace ssoa
{
    /// The top-level class of the registry server.
    class RegistryListener: private boost::noncopyable
    {
    public:
        /// Initializes an instance of the server, binding it to the specified TCP address and port,
        /// and starts listening for incoming connections.
        ///
        /// @param host A string identifying a host. May be a descriptive name or a numeric address
        /// string. If empty, the loopback address will be used.
        ///
        /// @param port A string containing the port number.
        ///
        /// @throws boost::system::system_error Thrown on failure.
        explicit RegistryListener(const std::string& host, const std::string& port, std::size_t thread_pool_size);

        /// Run the server's io_service loop.
        void run();

    private:
        /// Initiate an asynchronous accept operation.
        void startAccept();

        /// Handle completion of an asynchronous accept operation.
        void handleAccept(const boost::system::error_code& e);

        /// Handle a request to stop the server.
        void handleStop();

        /// The number of threads that will call io_service::run().
        std::size_t threadPoolSize;

        /// The io_service used to perform asynchronous operations.
        /// The io_service class it deals with the operating system, waiting for all asynchronous
        /// operations to end, and then calling the completion handler for each such operation.
        boost::asio::io_service ioService;

        /// The signal_set is used to register for process termination notifications.
        boost::asio::signal_set signals;

        /// Acceptor used to listen for incoming connections.
        boost::asio::ip::tcp::acceptor acceptor;

        /// The next connection to be accepted.

        std::shared_ptr<ClientHandler> clientHandler;
    };
}

#endif
