/*
 * registrylistener.cpp
 */

#include <ssoa/registry/registrylistener.h>

#include "clienthandler.h"

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using std::string;

namespace ssoa
{
    /// @details
    /// A handler is registered to process termination signals (SIGINT, SIGTERM and, if defined, SIGQUIT)
    /// and let the server quit gracefully. Notice that it is safe to register for the same signal multiple
    /// times in a program, provided all registrations for the specified signal are made through Asio.
    RegistryListener::RegistryListener(
        const string& host, const string& port, size_t thread_pool_size, IRegistry& registry) :
        threadPoolSize(thread_pool_size), signals(ioService), acceptor(ioService), clientHandler(), registry(registry)
    {
        signals.add(SIGINT);
        signals.add(SIGTERM);
#if defined(SIGQUIT)
        signals.add(SIGQUIT);
#endif
        signals.async_wait(boost::bind(&RegistryListener::handleStop, this));

        using namespace boost::asio::ip;
        tcp::resolver resolver(ioService);
        tcp::resolver::query query(host, port);
        tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor.open(endpoint.protocol());
        acceptor.set_option(tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();

        startAccept();
    }

    void RegistryListener::run()
    {
        // Create a pool of threads executing io_service::run().
        boost::thread_group threads;
        for (size_t i = 0; i < threadPoolSize; ++i) {
            threads.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
        }

        // By calling io_service::run() from all threads in the pool, we let the io_service to post
        // each asynchronous callback to any of these threads. This will block all threads until
        // all asynchronous operations complete, that is, the execution of io_service::run() ends
        // when the io_service has no more pending requests.

        // Notice that, at this point, we already called startAccept(), and hence we do have a
        // pending request.

        // On the main thread, we do not call io_service::run(), so no callback is posted and no
        // client is serviced here: the main thread just waits for all service threads to complete.
        threads.join_all();
    }

    void RegistryListener::startAccept()
    {
        // We can reset a shared_ptr to a ClientHandler since it registers its callbacks by using a
        // shared_from_this() pointer.
        clientHandler.reset(new ClientHandler(ioService, registry));

        // Registers for an asynchronous accept
        acceptor.async_accept(clientHandler->getSocket(),
            boost::bind(&RegistryListener::handleAccept, this, boost::asio::placeholders::error));
    }

    void RegistryListener::handleAccept(const boost::system::error_code& e)
    {
        if (!e) {
            clientHandler->start();
        }

        // Every time a new connection is accepted, we start listening again, so that
        // the io_service's queue doesn't get empty and threads keep being executed.
        startAccept();
    }

    void RegistryListener::handleStop()
    {
        ioService.stop();
    }
}
