/*
 * serviceskeleton.cpp
 */

#include <ssoa/service/serviceskeleton.h>
#include <ssoa/logger.h>
#include <ssoa/registry/registry.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
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
using boost::format;
using boost::system::error_code;

namespace ssoa
{
    class ServiceSkeletonSerializationHelper:
        public std::enable_shared_from_this<ServiceSkeletonSerializationHelper>, private boost::noncopyable
    {
    public:
        ServiceSkeletonSerializationHelper(std::unique_ptr<tcp::socket> socket) :
            socket(std::move(socket)), signature(ServiceSignature::any)
        {
        }

        void start() {
            Logger::debug() << format("%1% -- Accepted request.") % socket->remote_endpoint() << std::endl;
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

        void onHeaderReceived(const error_code& e, size_t bytes_transferred);
        void onPayloadReceived(const error_code& e, size_t bytes_transferred);
        void sendResponse(Response * r);
        void onWriteResponse(const error_code& e);
    };

    void ServiceSkeleton::start(std::unique_ptr<tcp::socket> socket)
    {
        std::shared_ptr<ServiceSkeletonSerializationHelper> helper(
            new ServiceSkeletonSerializationHelper(std::move(socket)));
        helper->start();
    }

    void ServiceSkeletonSerializationHelper::onHeaderReceived(const error_code& e, size_t bytes_transferred)
    {
        if (e) {
            string message("Cannot receive header: " + e.message());
            Logger::debug() << format("%1% -- %2%") % socket->remote_endpoint() % message << std::endl;
            sendResponse(new Response(signature, false, message));
            return;
        }

        Logger::debug() << format("%1% -- Header received.") % socket->remote_endpoint() << std::endl;

        try {
            streambuf::const_buffers_type bufs = headerBuffer.data();

            string header(buffers_begin(bufs), buffers_begin(bufs) + bytes_transferred - 1);
            istringstream ss(header);
            YAML::Parser parser(ss);
            YAML::Node node;
            parser.GetNextDocument(node);

            signature = node["service"].to<string>();

            // Validate the signature by checking if the provider actually supports the service
            if (!ServiceSkeleton::factory().contains(signature)) {
                sendResponse(new Response(signature, false, "Service not available."));
                // Avoid reading all arguments when the service is unavailable.
                // With each connection, a single service request is serviced, so the we
                // immediately send a response and close the socket (automatically done on return).
                return;
            }

            vector<unsigned int> blocks;
            const YAML::Node& blocksNode = node["blocks"];
            for (unsigned i = 0; i < blocksNode.size(); i++) {
                int block;
                blocksNode[i] >> block;
                blocks.push_back(block);
            }

            const auto& params = signature.getInputParams();
            if (blocks.size() != params.size()) {
                std::string bs = boost::lexical_cast<std::string>(blocks.size());
                std::string ps = boost::lexical_cast<std::string>(params.size());
                throw std::runtime_error(
                    "Received an invalid request (expected " + ps + " argument blocks, received " + bs + ").");
            }

            // We must now consume additional data contained in the streambuf beyond the delimiter '\0'
            // fetched by async_read_until().
            headerBuffer.consume(bytes_transferred);

            for (unsigned i = 0; i < params.size(); i++) {
                ServiceArgument *arg = ServiceArgument::prepare(params[i], blocks[i]);
                arguments.emplace_back(arg);
                auto bufdata = arg->getData();
                int fetched_size = headerBuffer.size();
                if (fetched_size > 0) {
                    boost::asio::buffer_copy(bufdata, headerBuffer.data());
                    payloadBuffers.push_back(bufdata + fetched_size);
                    headerBuffer.consume(blocks[i] < fetched_size ? blocks[i] : fetched_size);
                }
                else {
                    payloadBuffers.push_back(bufdata);
                }
            }

            async_read(*socket.get(), payloadBuffers,
                       boost::bind(&ServiceSkeletonSerializationHelper::onPayloadReceived,
                                   shared_from_this(),
                                   boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
        catch (const std::exception &e) {
            sendResponse(new Response(signature, false, e.what()));
        }
    }

    void ServiceSkeletonSerializationHelper::onPayloadReceived(const error_code& e, size_t bytes_transferred)
    {
        if (e) {
            string message("Cannot receive payload: " + e.message());
            Logger::debug() << format("%1% -- %2%") % socket->remote_endpoint() % message << std::endl;
            sendResponse(new Response(signature, false, message));
            return;
        }

        Logger::debug() << format("%1% -- Payload received.") % socket->remote_endpoint() << std::endl;

        Logger::debug() << format("%1% -- Preparing response.") % socket->remote_endpoint() << std::endl;
        unique_ptr<ServiceSkeleton> impl(ServiceSkeleton::factory().create(signature, std::move(arguments)));
        sendResponse(impl->invoke());
    }

    void ServiceSkeletonSerializationHelper::sendResponse(Response * r)
    {
        if (r == NULL) {
            r = new Response(signature, false, "Internal server error: produced a NULL response.");
        }
        response.reset(r);
        format fmt("%1% -- Sending response: %2%");
        Logger::debug() << fmt % socket->remote_endpoint() % response->getStatus() << std::endl;
        response->serialize(*socket.get(),
                            boost::bind(&ServiceSkeletonSerializationHelper::onWriteResponse,
                                        shared_from_this(),
                                        boost::asio::placeholders::error));
    }

    void ServiceSkeletonSerializationHelper::onWriteResponse(const error_code& e)
    {
        if (!e) {
            // Initiate graceful connection closure.
            error_code ignored_ec;
            socket->shutdown(tcp::socket::shutdown_both, ignored_ec);
        }
        else {
            Logger::debug() << e.message() << std::endl;
        }
    }
}
