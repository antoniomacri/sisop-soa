/*
 * registry.h
 */

#ifndef _REGISTRY_H_
#define _REGISTRY_H_

#include <ssoa/service/service.h>
#include <ssoa/registry/registrymessage.h>

#include <string>

namespace ssoa
{
    /// Implements a proxy which communicates to the registry server.
    ///
    /// This class is not thread-safe: address and port should be set just once at program startup.
    class Registry
    {
    public:
        /// Sets the address and the port of the registry.
        static void initialize(std::string host, std::string port) {
            Registry::host = host;
            Registry::port = port;
        }

        /// Registers a service on the registry.
        ///
        /// @param signature The signature of the service to be deregistered.
        /// @param host The host name or IP address of the service provider.
        /// @param port The port on which the service was provided.
        ///
        /// @throws boost::system::system_error If an error occurred while communicating
        ///         with the registry.
        /// @throws std::runtime_error An invalid response was received from registry, or
        ///         the response is unsuccessful.
        static void registerService(const ServiceSignature& signature, std::string host, std::string port) {
            registerService(signature, host, port, false);
        }

        /// Deregisters a service on the registry.
        ///
        /// @param signature The signature of the service to be deregistered.
        /// @param host The host name or IP address of the service provider.
        /// @param port The port on which the service was provided.
        ///
        /// @throws boost::system::system_error If an error occurred while communicating
        ///         with the registry.
        /// @throws std::runtime_error An invalid response was received from registry, or
        ///         the response is unsuccessful.
        static void deregisterService(const ServiceSignature& signature, std::string host, std::string port) {
            registerService(signature, host, port, false);
        }

        /// Deregisters all services of a provider.
        ///
        /// @param host The host name or IP address of the service provider.
        /// @param port The port on which the services were provided.
        ///
        /// @throws boost::system::system_error If an error occurred while communicating
        ///         with the registry.
        /// @throws std::runtime_error An invalid response was received from registry, or
        ///         the response is unsuccessful.
        static void deregisterProvider(std::string host, std::string port) {
            registerService(ServiceSignature::any, host, port, false);
        }

        /// Submits a message to the registry and waits for the response.
        ///
        /// @param message The message to send to the registry.
        ///
        /// @return The response received from the registry.
        ///
        /// @throws boost::system::system_error If an error occurred while communicating
        ///         with the registry.
        /// @throws std::runtime_error An invalid response was received from registry, or
        ///         the response is unsuccessful.
        static RegistryMessage * submit(const RegistryMessage& message);

        /// Obtains a host:port pair from the registry for the given service.
        ///
        /// @param signature The signature of the service to lookup for.
        ///
        /// @return A pair containing the host and port of the requested service.
        ///
        /// @throws boost::system::system_error If an error occurred while communicating
        ///         with the registry.
        /// @throws std::runtime_error An invalid response was received from registry, or
        ///         the response is unsuccessful.
        static std::pair<std::string, std::string> getProvider(std::string signature);

    private:
        static std::string host;
        static std::string port;

        /// Prevent instantiation.
        Registry()
        {
        }

        /// Registers or deregisters a service on the Registry.
        static void registerService(const ServiceSignature& signature, std::string host, std::string port,
            bool deregister);
    };
}

#endif
