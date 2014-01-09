/*
 * listener.h
 */

#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <ssoa/service/service.h>

#include <string>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

namespace storageprovider
{
    using namespace ssoa;

    /// Represents a listening server.
    class Listener: private boost::noncopyable
    {
    public:
        /// Initializes an instance of the server and starts listening for incoming connections.
        ///
        /// @param host A string identifying a host. May be a descriptive name or a numeric
        ///        address string. If empty, the loopback address will be used.
        /// @param port A string containing the port number.
        /// @param thread_pool_size The number of threads allocated in the pool.
        ///
        /// @throws boost::system::system_error Thrown on failure.
        explicit Listener(const std::string& host, const std::string& port, std::size_t thread_pool_size);

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

        /// The next client socket.
        std::unique_ptr<boost::asio::ip::tcp::socket> clientSocket;
    };
}

#endif
