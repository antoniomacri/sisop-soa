/*
 * clienthandler.cpp
 */

#include "clienthandler.h"

#include <logger.h>
#include <registryerrormessage.h>
#include <registryregistrationresponse.h>
#include <registryserviceresponse.h>

#include <vector>
#include <fstream>

#include <boost/asio/placeholders.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

using std::string;
using namespace boost::asio;
using namespace boost::asio::ip;

namespace ssoa
{
    ClientHandler::ClientHandler(io_service& io_service) :
        socket(io_service)
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
            std::cout << "Request:\n====\n" << text << "\n===" << std::endl;

            try {
                std::unique_ptr<RegistryMessage> req(RegistryMessage::fromYaml(text));
                response = generateResponse(req.get());
            }
            catch (std::runtime_error& e) {
                // An error dependent on the request (e.g., malformed YAML).
                response = RegistryErrorMessage(e.what()).toYaml();
            }
            catch (std::exception& e) {
                // This should be an internal server error: log it.
                Logger::error() << e.what() << std::endl;
                response = RegistryErrorMessage(e.what()).toYaml();
            }

            std::cout << "Reply:\n====\n" << response << "\n===" << std::endl;
            async_write(socket, boost::asio::buffer(response.c_str(), response.size() + 1),
                        boost::bind(&ClientHandler::handleWrite, shared_from_this(),
                                    boost::asio::placeholders::error));
        }
        else {
            Logger::error() << e.message() << std::endl;
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
            Logger::error() << e.message() << std::endl;
        }

        // No new asynchronous operations are started. This means that all shared_ptr
        // references to the connection object will disappear and the object will be
        // destroyed automatically after this handler returns. The connection class's
        // destructor closes the socket.
    }

    string ClientHandler::generateResponse(RegistryMessage *request)
    {
        RegistryMessage::Type type = request->getType();
        switch (type) {
            case RegistryMessage::TYPE_REGISTRATION_REQUEST:
                return generateRegistrationResponse(dynamic_cast<RegistryRegistrationRequest*>(request));
            case RegistryMessage::TYPE_SERVICE_REQUEST:
                return generateServiceResponse(dynamic_cast<RegistryServiceRequest*>(request));
            case RegistryMessage::TYPE_INVALID:
                case RegistryMessage::TYPE_SERVICE_RESPONSE:
                case RegistryMessage::TYPE_REGISTRATION_RESPONSE:
                case RegistryMessage::TYPE_ERROR:
                // Avoid warning
                break;
        }
        return RegistryErrorMessage("Unsupported request (type code: "
                                    + boost::lexical_cast<string>((int)type)
                                    + ").").toYaml();
    }

    string ClientHandler::generateRegistrationResponse(RegistryRegistrationRequest *request)
    {
        std::cout << "handling registration request" << std::endl;
        string service = request->getService();
        string host = request->getHost();
        string port = request->getPort();
        bool unregister = request->getUnregister();
        try {
            bool result;
            if (unregister)
                result = registry.unregisterService(ServiceSignature(service), host, port);
            else
                result = registry.registerService(ServiceSignature(service), host, port);
            return RegistryRegistrationResponse(result).toYaml();
        }
        catch (std::exception& e) {
            return RegistryErrorMessage(e.what()).toYaml();
        }
    }

    string ClientHandler::generateServiceResponse(RegistryServiceRequest *request)
    {
        std::cout << "handling service request" << std::endl;
        string service = request->getService();
        std::cout << "service: " << service << std::endl;
        try {
            string host, port;
            if (registry.lookupService(ServiceSignature(service), host, port)) {
                std::cout << "Found!" << std::endl;
                std::cout << "  host: " << host << std::endl;
                std::cout << "  port: " << port << std::endl;
                return RegistryServiceResponse(host, port).toYaml();
            }
            else {
                std::cout << "Not found!" << std::endl;
                return RegistryServiceResponse("No provider available for the requested service.").toYaml();
            }
        }
        catch (std::exception& e) {
            return RegistryErrorMessage(e.what()).toYaml();
        }
    }
}
