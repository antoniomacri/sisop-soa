/*
 * registrymessage.h
 */

#ifndef _REGISTRYMESSAGE_H_
#define _REGISTRYMESSAGE_H_

#include <servicesignature.h>

#include <string>

#include <boost/algorithm/string/replace.hpp>

using std::string;

// Forward-declare YAML::Node
namespace YAML
{
    class Node;
}

namespace ssoa
{
    class RegistryMessage
    {
    public:
        enum Type
        {
            TYPE_INVALID = 0,
            TYPE_SERVICE_REQUEST,
            TYPE_SERVICE_RESPONSE,
            TYPE_REGISTRATION_REQUEST,
            TYPE_REGISTRATION_RESPONSE,
            TYPE_ERROR,
            TYPE_LAST = TYPE_ERROR
        };

        Type getType() {
            return type;
        }

        /// Constructs a new RegistryMessage from the given YAML string.
        /// Only the first YAML document is read, others are ignored.
        ///
        /// @param A string containing the YAML code.
        ///
        /// @return Never returns NULL (in case of error an exception is thrown).
        ///
        /// @throws YAML::Exception if @c yaml contains invalid YAML code.
        static RegistryMessage * fromYaml(const string & yaml);

        virtual string toYaml() const = 0;

        virtual ~RegistryMessage()
        {
        }

    protected:
        RegistryMessage(Type type) :
            type(type)
        {
        }

        // TODO: improve escape/unescape
        static string escapeYaml(string text) {
            if (text.find_first_of(":'\"{}[]") == string::npos)
                return text;
            return "'" + boost::algorithm::replace_all_copy(text, "'", "\\'") + "'";
        }
        static string unescapeYaml(string text) {
            if (text.size() > 0 && text[0] == '\'' && text[text.size() - 1] == '\'')
                return boost::algorithm::replace_all_copy(text.substr(0, text.size() - 2), "\\'", "'");
            return text;
        }

    private:
        Type type;
    };

    class RegistryErrorMessage: public RegistryMessage
    {
    public:
        /// @param status It will be escaped
        RegistryErrorMessage(const string & status) :
            RegistryMessage(TYPE_ERROR), status(escapeYaml(status))
        {
        }

        const string & getStatus() const {
            return status;
        }

        static RegistryErrorMessage * fromYaml(const YAML::Node& node);
        virtual string toYaml() const;

    private:
        string status;
    };

    class RegistryRegistrationRequest: public RegistryMessage
    {
    public:
        RegistryRegistrationRequest(const ServiceSignature & service, string host, string port, bool unregister = false) :
            RegistryMessage(TYPE_REGISTRATION_REQUEST),
                service(service), host(std::move(host)), port(std::move(port)), unregister(unregister)
        {
        }

        const ServiceSignature & getService() const {
            return service;
        }

        const string & getHost() const {
            return host;
        }

        const string & getPort() const {
            return port;
        }

        bool getUnregister() const {
            return unregister;
        }

        static RegistryRegistrationRequest * fromYaml(const YAML::Node& node);
        virtual string toYaml() const;

    private:
        ServiceSignature service;
        string host;
        string port;
        bool unregister;
    };

    class RegistryRegistrationResponse: public RegistryMessage
    {
    public:
        /// @param status It will be escaped
        RegistryRegistrationResponse(
            const RegistryRegistrationRequest * request, bool successful = true, const string & status = "") :
            RegistryMessage(TYPE_REGISTRATION_RESPONSE),
                request(request), successful(successful), status(escapeYaml(status))
        {
        }

        const RegistryRegistrationRequest * getRequest() const {
            // TODO: it is really useful?
            return request;
        }

        bool isSuccessful() const {
            return successful;
        }

        const string & getStatus() const {
            return status;
        }

        static RegistryRegistrationResponse * fromYaml(const YAML::Node& node);
        virtual string toYaml() const;

    private:
        const RegistryRegistrationRequest * request;
        const bool successful;
        const string status;
    };

    class RegistryServiceRequest: public RegistryMessage
    {
    public:
        RegistryServiceRequest(const ServiceSignature & service) :
            RegistryMessage(TYPE_SERVICE_REQUEST), service(service)
        {
        }

        const ServiceSignature & getService() const {
            return service;
        }

        static RegistryServiceRequest * fromYaml(const YAML::Node& node);
        virtual string toYaml() const;

    private:
        const ServiceSignature service;
    };

    class RegistryServiceResponse: public RegistryMessage
    {
    public:
        /// @param status It will be escaped
        RegistryServiceResponse(const RegistryServiceRequest * request, string host, string port) :
            RegistryMessage(TYPE_SERVICE_RESPONSE),
                request(request), host(std::move(host)), port(std::move(port)), successful(true)
        {
        }

        /// @param status It will be escaped
        RegistryServiceResponse(const RegistryServiceRequest * request, const string & status) :
            RegistryMessage(TYPE_SERVICE_RESPONSE),
                request(request), host(""), port(""), successful(false), status(escapeYaml(status))
        {
        }

        // TODO: it is really useful?
        const RegistryServiceRequest * getRequest() const {
            return request;
        }

        const string & getHost() const {
            return host;
        }

        const string & getPort() const {
            return port;
        }

        bool isSuccessful() const {
            return successful;
        }

        const string & getStatus() const {
            return status;
        }

        static RegistryServiceResponse * fromYaml(const YAML::Node& node);
        virtual string toYaml() const;

    private:
        const RegistryServiceRequest * request;
        const string host;
        const string port;
        const bool successful;
        const string status;
    };
}

#endif