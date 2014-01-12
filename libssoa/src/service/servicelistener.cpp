/*
 * servicelistener.cpp
 */

#include <ssoa/service/servicelistener.h>
#include <ssoa/service/serviceskeleton.h>

#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using std::string;
using namespace boost::asio::ip;

namespace ssoa
{
    ServiceListener::ServiceListener(const string& host, const string& port, size_t thread_pool_size) :
        threadPoolSize(thread_pool_size), signals(ioService), acceptor(ioService)
    {
        signals.add(SIGINT);
        signals.add(SIGTERM);
#if defined(SIGQUIT)
        signals.add(SIGQUIT);
#endif
        signals.async_wait(boost::bind(&ServiceListener::handleStop, this));

        tcp::resolver resolver(ioService);
        tcp::resolver::query query(host, port);
        tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor.open(endpoint.protocol());
        acceptor.set_option(tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();

        startAccept();
    }

    void ServiceListener::run()
    {
        boost::thread_group threads;
        for (size_t i = 0; i < threadPoolSize; ++i) {
            threads.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
        }

        threads.join_all();
    }

    void ServiceListener::startAccept()
    {
        clientSocket.reset(new tcp::socket(ioService));

        acceptor.async_accept(*clientSocket.get(), boost::bind(&ServiceListener::handleAccept, this,
                                                               boost::asio::placeholders::error));
    }

    void ServiceListener::handleAccept(const boost::system::error_code& e)
    {
        if (!e) {
            ServiceSkeleton::start(std::move(clientSocket));
        }

        startAccept();
    }

    void ServiceListener::handleStop()
    {
        ioService.stop();
    }
}
