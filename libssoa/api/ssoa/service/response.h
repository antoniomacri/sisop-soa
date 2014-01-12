/*
 * response.h
 */

#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <ssoa/service/servicesignature.h>
#include <ssoa/service/serviceargument.h>

#include <deque>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <yaml-cpp/yaml.h>

namespace ssoa
{
    /// Represents a response received from a service provider.
    class Response
    {
    public:
        /// Constructs a new successful Response.
        ///
        /// @param signature The signature of the requested service.
        Response(ServiceSignature signature) :
            signature(std::move(signature)), successful(true), status("OK"), pushed(0)
        {
        }

        /// Constructs a new Response.
        ///
        /// @param signature The signature of the requested service.
        /// @param successful A value indicating whether the operation is successful.
        /// @param status A string representing the status of the operation.
        Response(ServiceSignature signature, bool successful, std::string status) :
            signature(std::move(signature)), successful(successful), status(std::move(status)), pushed(0)
        {
        }

        /// Gets the signature of the requested service.
        const ServiceSignature& getSignature() const {
            return signature;
        }

        /// Gets a boolean value indicating whether the operation is successful.
        bool isSuccessful() const {
            return successful;
        }

        /// Gets a string representing the status of the operation.
        const std::string& getStatus() const {
            return status;
        }

        /// Adds an argument to the list of output arguments.
        ///
        /// @tparam ServArg The actual type of the argument (derived of ServiceArgument).
        ///         The template is only used to check the value returned by its @c type() method.
        ///
        /// @param arg A pointer to a ServiceArgument object which has to be added.
        ///        The ownership of @c arg is transferred to this Response instance.
        ///
        /// @throws std::logic_error If all arguments have already been pushed, or if the given
        ///         ServiceArgument does not match the one stated in the signature.
        template<class ServArg>
        void pushArgument(ServArg *arg)
        {
            if (pushed == signature.getOutputParams().size()) {
                throw std::logic_error("All arguments already pushed.");
            }
            std::string expected = signature.getOutputParams()[pushed];
            if (expected != ServArg::type()) {
                throw std::logic_error("Invalid argument (must be '" + expected + "').");
            }
            arguments.emplace_back(arg);
            pushed++;
        }

        /// Gets an argument from the list of output arguments and removes it from the list.
        ///
        /// @tparam ServArg The actual type of the argument (derived of ServiceArgument).
        ///
        /// @return A pointer to the retrieved ServiceArgument object.
        ///         The ownership of @c arg is transferred to the caller.
        ///
        /// @throws std::logic_error If not all arguments have been pushed yet, or if all
        ///         arguments have already been popped, or if the specified ServiceArgument
        ///         does not match the one stated in the signature.
        template<class ServArg>
        ServArg* popArgument()
        {
            if (pushed != signature.getOutputParams().size()) {
                throw std::logic_error("Not all arguments have been pushed.");
            }
            if (arguments.size() <= 0) {
                throw std::logic_error("All arguments already popped.");
            }

            ServiceArgument *a = arguments.front().get();
            ServArg *result = dynamic_cast<ServArg*>(a);
            if (result == NULL) {
                int index = signature.getOutputParams().size() - arguments.size();
                std::string expected = signature.getOutputParams()[index];
                throw std::logic_error("Invalid argument (must be '" + expected + "').");
            }
            arguments.front().release();
            arguments.pop_front();
            return result;
        }

        /// Performs synchronous deserialization of a Response.
        ///
        /// @param s The stream from which the data is to be read.
        ///        The type must support the SyncReadStream concept (see boost documentation)
        ///
        /// @return A new instance of the Response class deserialized from the given stream.
        ///
        /// @see boost::asio::read
        template<typename SyncReadStream>
        static Response * deserialize(SyncReadStream& s)
        {
            boost::asio::streambuf headerBuffer;
            size_t bytes_transferred = boost::asio::read_until(s, headerBuffer, '\0');
            boost::asio::streambuf::const_buffers_type bufs = headerBuffer.data();

            std::string header(buffers_begin(bufs), buffers_begin(bufs) + bytes_transferred - 1);
            std::istringstream ss(header);
            YAML::Parser parser(ss);
            YAML::Node node;
            parser.GetNextDocument(node);

            ServiceSignature signature = node["service"].to<std::string>();
            bool successful = node["successful"].to<bool>();
            std::string status = node["status"].to<std::string>();

            std::vector<unsigned int> blocks;
            const YAML::Node & blocksNode = node["blocks"];
            for (unsigned i = 0; i < blocksNode.size(); i++) {
                int block;
                blocksNode[i] >> block;
                blocks.push_back(block);
            }

            if (successful == false) {
                return new Response(signature, false, status);
            }

            const auto& params = signature.getOutputParams();
            if (blocks.size() != params.size()) {
                std::string bs = boost::lexical_cast<std::string>(blocks.size());
                std::string ps = boost::lexical_cast<std::string>(params.size());
                throw std::runtime_error(
                    "Received an invalid response (expected " + ps + " argument blocks, received " + bs + ").");
            }

            // We must now consume additional data contained in the streambuf beyond the delimiter '\0'
            // specified to async_read_until().
            headerBuffer.consume(bytes_transferred);

            Response *response = new Response(signature, successful, status);

            std::vector<boost::asio::mutable_buffer> payloadBuffers;
            for (unsigned i = 0; i < params.size(); i++) {
                ServiceArgument *arg = ServiceArgument::prepare(params[i], blocks[i]);
                response->arguments.emplace_back(arg);
                response->pushed++;
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

            boost::asio::read(s, payloadBuffers);
            return response;
        }

        /// Performs asynchronous serialization of this Response instance.
        ///
        /// @param stream The stream to which the data is to be written. The type must support
        ///        the AsyncWriteStream concept (see boost documentation). The object must
        ///        outlive the completion of the operation.
        /// @param handler The handler to be called when the serialization completes.
        ///
        /// @see boost::asio::async_write
        template<typename AsyncWriteStream, typename WriteHandler>
        void serialize(AsyncWriteStream& stream, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
        {
            boost::asio::async_write(stream, getConstBuffers(), handler);
        }

    protected:
        /// Just a shortcut.
        typedef std::deque<std::unique_ptr<ServiceArgument>> arg_deque;

        /// Builds a ConstBufferSequence which can be used to serialize this Response.
        ///
        /// The Response keeps ownership of memory referred to by all buffers, which can
        /// can be invalidated by any non-const method invoked on this instance.
        std::vector<boost::asio::const_buffer> getConstBuffers() const;

    private:
        ServiceSignature signature;
        bool successful;
        std::string status;
        arg_deque arguments;
        int pushed;
        mutable std::string header;    // Temporarily keeps the header
    };
}

#endif
