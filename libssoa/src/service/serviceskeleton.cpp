/*
 * serviceskeleton.cpp
 */

#include <service/serviceskeleton.h>
#include <logger.h>
#include <registry/registry.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <yaml-cpp/yaml.h>

using std::unique_ptr;
using std::istringstream;
using std::string;
using std::vector;

using boost::asio::buffers_begin;
using boost::asio::ip::tcp;
using boost::asio::mutable_buffer;
using boost::asio::streambuf;

namespace ssoa
{
    class ServiceSkeletonSerializationHelper:
        public std::enable_shared_from_this<ServiceSkeletonSerializationHelper>, private boost::noncopyable
    {
    public:
        ServiceSkeletonSerializationHelper(std::unique_ptr<tcp::socket> socket) :
            socket(std::move(socket)), signature(ServiceSignature::any)
        {
            async_read_until(
                *socket.get(),
                headerBuffer,
                '\0',
                boost::bind(&ServiceSkeletonSerializationHelper::onHeaderReceived,
                            shared_from_this(),
                            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        unique_ptr<tcp::socket> socket;
        streambuf headerBuffer;
        vector<mutable_buffer> payloadBuffers;

        ServiceSignature signature;
        ServiceSkeleton::arg_deque arguments;
        unique_ptr<Response> response;

        void onHeaderReceived(const boost::system::error_code& e, size_t bytes_transferred);
        void onPayloadReceived(const boost::system::error_code& e);
        void sendResponse(Response * r);
        void onWriteResponse(const boost::system::error_code& e);
    };

    void ServiceSkeleton::start(std::unique_ptr<tcp::socket> socket)
    {
        new ServiceSkeletonSerializationHelper(std::move(socket));
    }

    void ServiceSkeletonSerializationHelper::sendResponse(Response * r)
    {
        response.reset(r);
        response->serialize(*socket.get(),
                            boost::bind(&ServiceSkeletonSerializationHelper::onWriteResponse,
                                        shared_from_this(),
                                        boost::asio::placeholders::error));
    }

    void ServiceSkeletonSerializationHelper::onWriteResponse(const boost::system::error_code& e)
    {
        if (e) {
            Logger::debug() << e.message() << std::endl;
        }
    }

    void ServiceSkeletonSerializationHelper::onHeaderReceived(
        const boost::system::error_code& e, size_t bytes_transferred)
    {
        if (e) {
            sendResponse(new Response(signature, false, e.message()));
            return;
        }

        try {
            streambuf::const_buffers_type bufs = headerBuffer.data();

            string header(buffers_begin(bufs), buffers_begin(bufs) + bytes_transferred);
            istringstream ss(header);
            YAML::Parser parser(ss);
            YAML::Node node;
            parser.GetNextDocument(node);

            signature = node["service"].to<string>();

            // Validate the signature by checking if the provider actually supports the service
            if (!ServiceSkeleton::Factory::contains(signature.getName())) {
                sendResponse(new Response(signature, false, "Service not available."));
                // Avoid reading all arguments when the service is unavailable.
                // With each connection, a single service request is serviced, so the we
                // immediately send a response and close the socket (automatically done on return).
                return;
            }

            vector<int> blocks;
            const YAML::Node& blocksNode = node["blocks"];
            for (unsigned i = 0; i < blocksNode.size(); i++) {
                int block;
                blocksNode[i] >> block;
                blocks.push_back(block);
            }

            auto& params = signature.getInputParams();
            for (unsigned i = 0; i < params.size(); i++) {
                ServiceArgument *arg = ServiceArgument::prepare(params[i], blocks[i]);
                arguments.push_back(unique_ptr<ServiceArgument>(arg));
                payloadBuffers.push_back(arg->getData());
            }
            async_read(*socket.get(), payloadBuffers,
                       boost::bind(&ServiceSkeletonSerializationHelper::onPayloadReceived,
                                   shared_from_this(),
                                   boost::asio::placeholders::error));
        }
        catch (std::exception &e) {
            sendResponse(new Response(signature, false, e.what()));
        }
    }

    void ServiceSkeletonSerializationHelper::onPayloadReceived(const boost::system::error_code& e)
    {
        if (e) {
            sendResponse(new Response(signature, false, e.message()));
            return;
        }

        ServiceSkeleton *impl = ServiceSkeleton::Factory::create(signature, std::move(arguments));
        sendResponse(impl->invoke());
    }
}
