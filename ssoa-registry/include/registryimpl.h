/*
 * registryimpl.h
 */

#ifndef _REGISTRYIMPL_H_
#define _REGISTRYIMPL_H_

#include <ssoa/service/servicesignature.h>

#include <list>
#include <map>
#include <string>

#include <boost/thread/shared_mutex.hpp>

namespace ssoa
{
    /// Implements a service registry.
    class RegistryImpl
    {
    public:
        /// Registers a service with a given host:port pair.
        ///
        /// @param signature The signature of the service being registered.
        /// @param host The host name or IP address of the service provider.
        /// @param port The port on which the service provider listens for incoming requests.
        ///
        /// @return @c true if the service has been actually registered or @c false otherwise.
        ///         A return value of @c false does not mean an error (for instance, the service
        ///         was already registered with the same host and port).
        ///
        /// The given service signature must be valid and no service with the same name and
        /// different signature must be already registered (in both cases, an exception will
        /// be thrown). No check is made neither on the host string nor on the port.
        ///
        /// @throws std::runtime_error Thrown on error.
        bool registerService(const ServiceSignature& signature, std::string host, std::string port);

        /// Removes the specified service.
        ///
        /// @param signature The signature of the service being deregistered.
        /// @param host The host name or IP address of the service provider.
        /// @param port The port on which the service was provided.
        ///
        /// @return @c true if the service has been actually deregistered or @c false otherwise.
        ///         A return value of @c false does not mean an error (for instance, there was no
        ///         service registered with the same host and port).
        bool deregisterService(const ServiceSignature& signature, std::string host, std::string port);

        /// Removes all services registered with the given host and port.
        ///
        /// @param host The host name or IP address of the service provider.
        /// @param port The port on which the services were provided.
        ///
        /// @return The number of services found with with the specified host:port pair and
        ///         successfully deregistered.
        int deregisterServer(std::string host, std::string port);

        /// Searches for a service with the given signature.
        ///
        /// @param signature The service signature to search for.
        /// @param host A string that will contain the host of the service provider.
        /// @param port A string that will contain the port on which the service is provided.
        ///
        /// @return @c true if a match was found, @c false otherwise.
        bool lookupService(const ServiceSignature& signature, std::string& host, std::string& port);

    private:
        struct service_data {
            ServiceSignature signature;
            std::vector<std::pair<std::string, std::string>> providers;
            int lastUsed;
        };

        typedef std::map<const std::string, service_data> service_data_map;

        // Each service is uniquely identified by its name (parameters must match).
        service_data_map services;

        /// A mutex to protect integrity of the registry data structures.
        boost::shared_mutex mutex;
    };
}

#endif
