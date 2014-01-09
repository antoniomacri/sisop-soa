/*
 * serviceargument.h
 */

#ifndef _SERVICEARGUMENT_H_
#define _SERVICEARGUMENT_H_

#include <ssoa/factorybase.h>

#include <map>
#include <string>

#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/lexical_cast.hpp>

namespace ssoa
{
    /// Represents an argument in a service request or response.
    class ServiceArgument
    {
    public:
        /// Gets a const_buffer containing the data to be used to serialize the argument.
        virtual boost::asio::const_buffer getData() const = 0;

        /// Gets a mutable_buffer containing the data to be used to deserialize the argument.
        virtual boost::asio::mutable_buffer getData() = 0;

        /// Virtual destructor.
        virtual ~ServiceArgument() {
        }

    public:
        /// Just a shortcut.
        typedef FactoryBase<ServiceArgument, const size_t> Factory;

        /// Gets an identifier of the ServiceArgument hierarchy (used by FactoryBase).
        static const char * hierarchyName() {
            return stringify(ServiceArgument);
        }

        /// Constructs a new argument of the given type allocating a buffer of the size specified.
        ///
        /// The @c size argument is used to allocate an internal buffer of the argument which
        /// can then be used for efficient deserialization. This will avoid unnecessary copies.
        ///
        /// @param type The type of the argument.
        /// @param size The size of the data that the argument will contain.
        ///
        /// @return A newly created ServiceArgument with a buffer space of the given size.
        ///         The return value is never @c NULL.
        ///
        /// @throws std::runtime_error The type specified is unknown.
        static ServiceArgument* prepare(const std::string type, const size_t size) {
            return Factory::create(type, size);
        }

        /// Simplifies the installation of a new handler.
        ///
        /// @tparam T The class to register.
        ///
        /// The class @p T must meet two requirements:
        ///   1. must have a method @c type() returning a char array or a string
        ///      which contains the identifier of the type
        ///   2. must have a static method @c prepare() with the following signature:
        ///      @code ServiceArgument* prepare(const size_t size); @endcode
        ///
        /// @see prepare()
        ///      for further information about the behavior of @c prepare().
        ///
        /// @par Example
        /// To install a new handler, it is sufficient to add a line like the following
        /// at the end of the implementation file (.cpp) of each derived class:
        /// @code
        ///   static ssoa::ServiceArgument::installer<ServiceArgumentDerivedClass> install;
        /// @endcode
        template<typename T>
        struct installer
        {
            installer() {
                Factory::install(T::type(), T::prepare);
            }
        };

    protected:
        ServiceArgument() {
        }

        /// Throws an exception if the size @c current is different from @c expected.
        static void check_argument_size(size_t current, size_t expected) {
            if (current != expected) {
                throw std::runtime_error("Invalid argument size (must be "
                                         + boost::lexical_cast<std::string>(expected)
                                         + " instead of "
                                         + boost::lexical_cast<std::string>(current)
                                         + ").");
            }
        }
    };

    /// Represents an integer argument in a service request or response.
    class ServiceIntArgument: public ServiceArgument
    {
        int32_t value; // kept in network byte order

    public:
        /// Constructs a new instance of ServiceIntArgument.
        /// @param value The integer value (host byte-order) of this argument.
        ServiceIntArgument(int32_t value) :
            value(htonl(value))
        {
        }

        /// Constructs a new instance of ServiceIntArgument with a value of 0.
        ///
        /// The given @c size must match the size of a 32-bit integer.
        ///
        /// @param size Must be @c sizeof(int32_t).
        ///
        /// @return A newly created ServiceIntArgument.
        ///
        /// @throws std::runtime_error @c size is not equal to @c sizeof(int32_t).
        static ServiceArgument* prepare(size_t size) {
            check_argument_size(size, sizeof(value));
            return new ServiceIntArgument(0);
        }

        /// Gets the identifier of this type of argument (used for deserialization).
        static const char * type() {
            return "int";
        }

        /// Gets the value of the argument.
        int32_t getValue() const {
            return ntohl(value);
        }

        virtual boost::asio::const_buffer getData() const {
            return boost::asio::const_buffer(&value, sizeof(value));
        }

        virtual boost::asio::mutable_buffer getData() {
            return boost::asio::mutable_buffer(&value, sizeof(value));
        }
    };

    /// Represents an @c double argument in a service request or response.
    class ServiceDoubleArgument: public ServiceArgument
    {
        double value;

    public:
        /// Constructs a new instance of ServiceDoubleArgument.
        /// @param value The @c double value of this argument.
        ServiceDoubleArgument(double value) :
            value(value)
        {
        }

        /// Constructs a new instance of ServiceDoubleArgument with a value of 0.0.
        ///
        /// The given @c size must match the size of a @c double.
        ///
        /// @param size Must be @c sizeof(double).
        ///
        /// @return A newly created ServiceDoubleArgument.
        ///
        /// @throws std::runtime_error @c size is not equal to @c sizeof(double).
        static ServiceArgument* prepare(size_t size) {
            check_argument_size(size, sizeof(value));
            return new ServiceIntArgument(0.0);
        }

        /// Gets the identifier of this type of argument (used for deserialization).
        static const char * type() {
            return "double";
        }

        /// Gets the value of the argument.
        double getValue() const {
            return value;
        }

        virtual boost::asio::const_buffer getData() const {
            return boost::asio::const_buffer(&value, sizeof(value));
        }

        virtual boost::asio::mutable_buffer getData() {
            return boost::asio::mutable_buffer(&value, sizeof(value));
        }
    };

    /// Represents a @c string argument in a service request or response.
    class ServiceStringArgument: public ServiceArgument
    {
        std::string value;

        ServiceStringArgument(size_t size) {
            value.resize(size);
        }

    public:
        /// Constructs a new instance of ServiceStringArgument.
        /// @param value The string value of this argument.
        ServiceStringArgument(std::string value) :
            value(std::move(value))
        {
        }

        /// Constructs a new instance of ServiceStringArgument allocating space for a string
        /// of the given length.
        ///
        /// @param size The number of characters (bytes) that the string will contain.
        ///
        /// @return A newly created ServiceStringArgument.
        static ServiceArgument* prepare(size_t size) {
            return new ServiceStringArgument(size);
        }

        /// Gets the identifier of this type of argument (used for deserialization).
        static const char * type() {
            return "string";
        }

        /// Gets the value of the argument.
        const std::string& getValue() const {
            return value;
        }

        virtual boost::asio::const_buffer getData() const {
            return boost::asio::const_buffer(value.data(), value.size());
        }

        virtual boost::asio::mutable_buffer getData() {
            // Since C++11 a string is guaranteed to be contiguous in memory,
            // so we can prepare() it safely and use .data()
            return boost::asio::mutable_buffer(const_cast<char*>(value.data()), value.size());
        }
    };

    /// Represents a buffer given as argument in a service request or response.
    class ServiceBufferArgument: public ServiceArgument
    {
        typedef unsigned char byte;

        std::vector<byte> value;

        ServiceBufferArgument(size_t size) {
            value.resize(size);
        }

    public:
        /// Constructs a new instance of ServiceBufferArgument.
        /// @param value A vector of bytes representing a buffer stored in this argument.
        ServiceBufferArgument(std::vector<byte> value) :
            value(std::move(value))
        {
        }

        /// Constructs a new instance of ServiceBufferArgument allocating a buffer of the given size.
        ///
        /// @param size The number of bytes that the buffer will contain.
        ///
        /// @return A newly created ServiceBufferArgument.
        static ServiceArgument* prepare(size_t size) {
            return new ServiceBufferArgument(size);
        }

        /// Gets the identifier of this type of argument (used for deserialization).
        static const char * type() {
            return "buffer";
        }

        /// Gets the value of the argument.
        const std::vector<byte>& getValue() const {
            return value;
        }

        virtual boost::asio::const_buffer getData() const {
            return boost::asio::const_buffer(value.data(), value.size());
        }

        virtual boost::asio::mutable_buffer getData() {
            return boost::asio::mutable_buffer(value.data(), value.size());
        }
    };
}

#endif
