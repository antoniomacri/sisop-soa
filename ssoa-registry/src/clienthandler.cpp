/*
 * clienthandler.cpp
 */

#include "clienthandler.h"

#include <ssoa/logger.h>
#include <ssoa/registry/registryerrormessage.h>
#include <ssoa/registry/registryregistrationresponse.h>
#include <ssoa/registry/registryserviceresponse.h>

#include <vector>
#include <fstream>

#include <boost/asio/placeholders.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

using std::string;
using boost::format;
using namespace boost::asio;
using namespace boost::asio::ip;

namespace ssoa
{
    ClientHandler::ClientHandler(io_service& io_service, RegistryImpl& registry) :
        socket(io_service), registry(registry)
    {
    }

    tcp::socket& ClientHandler::getSocket()
    {
        return socket;
    }

    void ClientHandler::start()
    {
        async_read_until(socket, buffer, '\0',
                         boost::bind(&ClientHandler::handleRead, shared_from_this(),
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred));
    }

    void ClientHandler::handleRead(const boost::system::error_code& e, size_t bytes_transferred)
    {
        if (!e) {
            const char * begin = buffer_cast<const char*>(buffer.data());
            const char * end = begin + bytes_transferred - 1; // -1 to remove '\0'
            string text = std::string(begin, end);

            try {
                std::unique_ptr<RegistryMessage> req(RegistryMessage::fromYaml(text));
                response = generateResponse(req.get());
            }
            catch (std::runtime_error& e) {
                // An error dependent on the request (e.g., malformed YAML).
                Logger::error("Caught runtime_error: %1%.", e.what());
                response = RegistryErrorMessage(e.what()).toYaml();
            }
            catch (std::exception& e) {
                // This should be an internal server error: log it.
                Logger::error("Caught exception: %1%.", e.what());
                response = RegistryErrorMessage(e.what()).toYaml();
            }

            // After calling c_str(), the string is guaranteed to be null-terminated
            async_write(socket, boost::asio::buffer(response.c_str(), response.size() + 1),
                        boost::bind(&ClientHandler::handleWrite, shared_from_this(),
                                    boost::asio::placeholders::error));
        }
        else {
            Logger::error(e.message());
        }

        // If an error occurs then no new asynchronous operations are started. This
        // means that all shared_ptr references to the connection object will
        // disappear and the object will be destroyed automatically after this
        // handler returns. The connection class's destructor closes the socket.
    }

    void ClientHandler::handleWrite(const boost::system::error_code& e)
    {
        if (!e) {
            // Initiate graceful connection closure.
            boost::system::error_code ignored_ec;
            socket.shutdown(tcp::socket::shutdown_both, ignored_ec);
        }
        else {
            Logger::error(e.message());
        }

        // No new asynchronous operations are started. This means that all shared_ptr
        // references to the connection object will disappear and the object will be
        // destroyed automatically after this handler returns. The connection class's
        // destructor closes the socket.
    }

    string ClientHandler::generateResponse(RegistryMessage *request)
    {
        if (RegistryRegistrationRequest * regreq = dynamic_cast<RegistryRegistrationRequest*>(request)) {
            return generateRegistrationResponse(regreq);
        }
        if (RegistryServiceRequest * srvreq = dynamic_cast<RegistryServiceRequest*>(request)) {
            return generateServiceResponse(srvreq);
        }
        throw std::runtime_error("Unsupported request (class: " + string(typeid(request).name()) + ").");
    }

    string ClientHandler::generateRegistrationResponse(RegistryRegistrationRequest *request)
    {
        string service = request->getService();
        string host = request->getHost();
        string port = request->getPort();
        bool deregister = request->getDeregister();
        try {
            bool done;
            if (deregister) {
                done = registry.deregisterService(ServiceSignature(service), host, port);
            }
            else {
                done = registry.registerService(ServiceSignature(service), host, port);
            }
            Logger::info("%1% <%2%, %3%, %4%> -- <%5%, %6%>",
                         (deregister ? "-" : "+"),
                         service, host, port, done, "OK");
            return RegistryRegistrationResponse(true, "OK").toYaml();
        }
        catch (const std::exception& e) {
            Logger::info("%1% <%2%, %3%, %4%> -- %5%", (deregister ? "-" : "+"), service, host, port, e.what());
            return RegistryRegistrationResponse(e.what()).toYaml();
        }
    }

    string ClientHandler::generateServiceResponse(RegistryServiceRequest *request)
    {
        string service = request->getService();
        try {
            string host, port;
            if (registry.lookupService(ServiceSignature(service), host, port)) {
                Logger::info("* %1% -- <%2%, %3%>", service, host, port);
                return RegistryServiceResponse(host, port).toYaml();
            }
            else {
                Logger::info("* %1% -- Not found.", service);
                return RegistryServiceResponse("No provider available for the requested service.").toYaml();
            }
        }
        catch (const std::exception& e) {
            return RegistryServiceResponse(e.what()).toYaml();
        }
    }
}
