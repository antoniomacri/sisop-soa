/*
 * servicestub.cpp
 */

#include <service/servicestub.h>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>

using namespace boost::asio::ip;

namespace ssoa
{
    Response * ServiceStub::submit() const
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(host, port);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, resolver.resolve(query));

        boost::asio::write(socket, getConstBuffers());

        return Response::deserialize(socket);
    }
}
