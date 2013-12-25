#include "registry.h"
#include <registryregistrationrequest.h>
#include <registryregistrationresponse.h>
#include <registryservicerequest.h>
#include <registryserviceresponse.h>

#include <stdexcept>
#include <stdlib.h>

#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

namespace ssoa
{
    string Registry::host = "";
    string Registry::port = "";

    RegistryMessage * Registry::submit(const RegistryMessage & request)
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

    bool Registry::registerService(const ServiceSignature & signature, const string & host, const string & port)
    {
        RegistryRegistrationRequest request(signature, host, port);
        RegistryRegistrationResponse * response = dynamic_cast<RegistryRegistrationResponse*>(submit(request));
        bool successful = response != NULL && response->isSuccessful();
        delete response;
        return successful;
    }

    bool Registry::unregisterService(const ServiceSignature & signature, const string & host, const string & port)
    {
        RegistryRegistrationRequest request(signature, host, port, true);
        RegistryRegistrationResponse * response = dynamic_cast<RegistryRegistrationResponse*>(submit(request));
        bool successful = response != NULL && response->isSuccessful();
        delete response;
        return successful;
    }

    bool Registry::unregisterProvider(const string & host)
    {
        RegistryRegistrationRequest request(ServiceSignature::any, host, 0);
        RegistryRegistrationResponse * response = dynamic_cast<RegistryRegistrationResponse*>(submit(request));
        bool successful = response != NULL && response->isSuccessful();
        delete response;
        return successful;
    }

    Service* Registry::getService(const ServiceSignature & signature)
    {
        RegistryServiceResponse * response = (RegistryServiceResponse*)submit(RegistryServiceRequest(signature));
        if (response != NULL && response->isSuccessful()) {
            return new Service(signature, response->getHost(), response->getPort());
        }
        return NULL;
    }
}
