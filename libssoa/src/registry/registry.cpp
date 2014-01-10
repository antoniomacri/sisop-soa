/**
 * registry.cpp
 */

#include <ssoa/registry/registry.h>
#include <ssoa/registry/registryerrormessage.h>
#include <ssoa/registry/registryregistrationrequest.h>
#include <ssoa/registry/registryregistrationresponse.h>
#include <ssoa/registry/registryservicerequest.h>
#include <ssoa/registry/registryserviceresponse.h>

#include <stdexcept>
#include <stdlib.h>

#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

using std::string;

namespace ssoa
{
    string Registry::host = "";
    string Registry::port = "";

    RegistryMessage * Registry::submit(const RegistryMessage& request)
    {
        if (host.empty() || port.empty()) {
            throw std::logic_error("Registry not initialized!");
        }

        boost::asio::io_service service;
        boost::asio::ip::tcp::resolver r(service);
        boost::asio::ip::tcp::resolver::query q(host, port);
        boost::asio::ip::tcp::socket socket(service);
        boost::asio::connect(socket, r.resolve(q));

        string s = request.toYaml();
        boost::asio::write(socket, boost::asio::buffer(s.c_str(), s.size() + 1));

        boost::asio::streambuf buf;
        int count = read_until(socket, buf, '\0') - 1; // -1 to remove '\0'
        boost::asio::streambuf::const_buffers_type bufs = buf.data();
        s = string(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + count);
        return RegistryMessage::fromYaml(s);
    }

    void Registry::registerService(const ServiceSignature& signature, string host, string port, bool deregister)
    {
        RegistryRegistrationRequest request(signature, host, port, deregister);
        std::unique_ptr<RegistryMessage> received(submit(request));

        RegistryRegistrationResponse *response = dynamic_cast<RegistryRegistrationResponse*>(received.get());
        if (response != NULL) {
            if (response->isSuccessful()) {
                return;
            }
            throw std::runtime_error(response->getStatus());
        }
        RegistryErrorMessage *error = dynamic_cast<RegistryErrorMessage*>(received.get());
        if (error != NULL) {
            throw std::runtime_error(error->getStatus());
        }
        throw std::runtime_error("Received an invalid response from registry.");
    }

    std::pair<string, string> Registry::getProvider(string signature)
    {
        std::unique_ptr<RegistryMessage> received(submit(RegistryServiceRequest(signature)));
        RegistryServiceResponse * response = dynamic_cast<RegistryServiceResponse*>(received.get());
        if (response != NULL) {
            if (response->isSuccessful()) {
                return make_pair(response->getHost(), response->getPort());
            }
            throw std::runtime_error(response->getStatus());
        }
        RegistryErrorMessage *error = dynamic_cast<RegistryErrorMessage*>(received.get());
        if (error != NULL) {
            throw std::runtime_error(error->getStatus());
        }
        throw std::runtime_error("Received an invalid response from registry.");
    }
}
