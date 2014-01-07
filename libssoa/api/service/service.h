#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <service/response.h>
#include <service/servicesignature.h>

#include <deque>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

namespace ssoa
{
    typedef unsigned char byte;

    class Service
    {
    public:
        Service(const ServiceSignature & signature) :
            signature(signature), host(""), port("")
        {
        }

        Service(ServiceSignature signature, std::string host, std::string port) :
            signature(std::move(signature)), host(std::move(host)), port(std::move(port))
        {
        }

        void pushArgument(const int32_t * arg);
        void pushArgument(const double * arg);
        void pushArgument(const std::string * arg);
        void pushArgument(const std::vector<byte> * arg);

        void popArgument(int32_t * arg);
        void popArgument(double * arg);
        void popArgument(std::string * arg);
        void popArgument(std::vector<byte> * arg);

        Response * submit();
        static Service * fromStream(std::istream stream);

        const std::string & getName() const
        {
            return signature.getName();
        }

        const ServiceSignature & getSignature() const
        {
            return signature;
        }

        const std::string & getHost() const
        {
            return host;
        }

        const std::string & getPort() const
        {
            return port;
        }

    private:
        ServiceSignature signature;
        std::string host;
        std::string port;

        struct Argument
        {
            const void * value;
            const std::string type;
            const int size;

            Argument(const int32_t * arg) :
                value(arg), type("int"), size(sizeof(int32_t))
            {
            }
            Argument(const double * arg) :
                value(arg), type("double"), size(sizeof(double))
            {
            }
            Argument(const std::string * arg) :
                value(arg->data()), type("string"), size(sizeof(arg->size()))
            {
            }
            Argument(const std::vector<byte> * arg) :
                value(arg->data()), type("buffer"), size(sizeof(arg->size()))
            {
            }
            Argument(const void * value, std::string type, int size) :
                value(value), type(type), size(size)
            {
            }
        };

        std::deque<Argument> arguments;

        inline void throw_invalid_argument(int index) const;
        inline void throw_missing_arguments() const;
    };
}

#endif
