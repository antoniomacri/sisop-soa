/*
 * registrylistener.h
 */

#ifndef _REGISTRYLISTENER_H_
#define _REGISTRYLISTENER_H_

#include <string>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <ssoa/registry/iregistry.h>

namespace ssoa
{
    // Forward declaration.
    class ClientHandler;

    /// Represents the listening registry server.
    class RegistryListener: private boost::noncopyable
    {
    public:
        /// Initializes an instance of the server and starts listening for incoming connections.
        ///
        /// @param host A string identifying a host. May be a descriptive name or a numeric
        ///        address string. If empty, the loopback address will be used.
        /// @param port A string containing the port number.
        /// @param thread_pool_size The number of threads allocated in the pool.
        /// @param registry The actual registry implementation.
        ///
        /// @throws boost::system::system_error Thrown on failure.
        explicit RegistryListener(const std::string& host, const std::string& port, std::size_t thread_pool_size,
            IRegistry& registry);

        /// Runs the server's io_service loop.
        void run();

    private:
        /// Initiates an asynchronous accept operation.
        void startAccept();

        /// Handles completion of an asynchronous accept operation.
        void handleAccept(const boost::system::error_code& e);

        /// Handles a request to stop the server.
        void handleStop();

        /// The number of threads that will call io_service::run().
        std::size_t threadPoolSize;

        /// The io_service used to perform asynchronous operations.
        boost::asio::io_service ioService;

        /// The signal_set is used to register for process termination notifications.
        boost::asio::signal_set signals;

        /// Acceptor used to listen for incoming connections.
        boost::asio::ip::tcp::acceptor acceptor;

        /// The next connection to be accepted.
        std::shared_ptr<ClientHandler> clientHandler;

        /// An instance of the actual registry implementation.
        IRegistry& registry;
    };
}

#endif
