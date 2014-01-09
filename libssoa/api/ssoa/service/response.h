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
            signature(std::move(signature)), successful(true), status("OK")
        {
        }

        /// Constructs a new Response.
        ///
        /// @param signature The signature of the requested service.
        /// @param successful A value indicating whether the operation is successful.
        /// @param status A string representing the status of the operation.
        Response(ServiceSignature signature, bool successful, std::string status) :
            signature(std::move(signature)), successful(successful), status(std::move(status))
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
        ///         The template is only used to check the @c type() value.
        ///
        /// @param arg A pointer to a ServiceArgument object which has to be added.
        ///        The ownership of @c arg is transferred to this Response instance.
        ///
        /// @throws std::logic_error If the given ServiceArgument does not match the
        ///         one specified in the signature.
        template<class ServArg>
        void pushArgument(const ServArg *arg)
        {
            if (signature.getInputParams()[arguments.size()] != ServArg::type()) {
                std::string expected = signature.getInputParams()[arguments.size()];
                throw std::logic_error("Invalid argument (must be '" + expected + "').");
            }
            arguments.push_back(arg);
        }

        /// Gets an argument from the list of output arguments and removes it from the list.
        ///
        /// @tparam ServArg The actual type of the argument (derived of ServiceArgument).
        ///
        /// @return A pointer to the retrieved ServiceArgument object.
        ///         The ownership of @c arg is transferred to the caller.
        ///
        /// @throws std::logic_error If the specified ServiceArgument does not match the
        ///         one stated in the signature.
        template<class ServArg>
        const ServArg* popArgument()
        {
            ServiceArgument *a = arguments.front();
            ServArg *result = dynamic_cast<ServArg*>(a);
            if (result == NULL) {
                std::string expected = signature.getInputParams()[0];
                throw std::logic_error("Invalid argument (must be '" + expected + "').");
            }
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
            boost::asio::streambuf streambuf;
            size_t n = boost::asio::read_until(s, streambuf, '\0');
            boost::asio::streambuf::const_buffers_type bufs = streambuf.data();
            std::string header(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + n);

            std::istringstream ss(header);
            YAML::Parser parser(ss);
            YAML::Node node;
            parser.GetNextDocument(node);

            ServiceSignature signature = node["service"].to<std::string>();
            bool successful = node["successful"].to<bool>();
            std::string status = node["status"].to<std::string>();

            std::vector<int> blocks;
            const YAML::Node & blocksNode = node["blocks"];
            for (unsigned i = 0; i < blocksNode.size(); i++) {
                int block;
                blocksNode[i] >> block;
                blocks.push_back(block);
            }

            Response *response = new Response(signature, successful, status);
            auto& params = signature.getOutputParams();
            std::vector<boost::asio::mutable_buffer> buffers;
            for (unsigned i = 0; i < params.size(); i++) {
                ServiceArgument *arg = ServiceArgument::prepare(params[i], blocks[i]);
                response->arguments.push_back(std::unique_ptr<ServiceArgument>(arg));
                buffers.push_back(arg->getData());
            }

            boost::asio::read(s, buffers);
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
        typedef std::deque<std::unique_ptr<const ServiceArgument>> arg_deque;

        /// Builds a ConstBufferSequence which can be used to serialize this Response.
        std::vector<boost::asio::const_buffer> getConstBuffers() const;

    private:
        ServiceSignature signature;
        bool successful;
        std::string status;
        arg_deque arguments;
    };
}

#endif
