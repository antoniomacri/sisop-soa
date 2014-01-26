
sisop-soa
=========

An infrastructure for a simplified Service Oriented Architecture.

Academic project, _Sistemi Operativi e Programmazione Distribuita_, Università di Pisa.


Overview
========

This project is written in C++, following the C++11 standard, using GCC, `make` and `doxygen`.

Directory structure:

  * `libssoa/`: contains all source files of the library
  * `libssoa-test/`: contains a few tests on the library
  * `ssoa-registry/`: contains source files of registry
  * `ssoa-registry-test/`: contains a few tests on the registry
  * `ssoa-imagemanipulationprovider/`: contains source code of a service provider used to manupulate images
  * `ssoa-storageprovider/`: contains source code of a storage service provider
  * `ssoa-client/`: contains source code of an example client
  * `testcase/`: contains source files of a test program
  * `doc/`: will contain documentation produced by `Doxygen` and a PDF report written in LaTeX
  * `bin/`: will contain all executables
  * `lib/`: will contain the static library `libssoa.a`
  * `Doxyfile`: configuration file for `Doxygen`
  * `Makefile`: used by `make` to build everything


Compiling
---------

 *Target*                    | *Group* | *Output*
-----------------------------|:-------:|--------------------------------------
 `library`                   | `all`   | `lib/libssoa.a`
 `registry`                  | `all`   | `bin/ssoa-registry`
 `storageprovider`           | `all`   | `bin/ssoa-storageprovider`
 `imagemanipulationprovider` | `all`   | `bin/ssoa-imagemanipulationprovider`
 `client`                    | `all`   | `bin/ssoa-client`
 `test-library`              | `test`  | `bin/libssoa-test`
 `test-registry`             | `test`  | `bin/ssoa-registry-test`
 `testcase`                  | –       | `bin/testcase`
 `documentation`             | –       | `doc/html/...`
 `report`                    | –       | `doc/ssoa-report.pdf` (_in italian_)
 `clean`                     | –       | Remove object and dependency files
 `distclean`                 | –       | Remove all generated files

Dependencies:

  * `libboost-dev` `libboost-filesystem-dev` `libboost-program-options-dev` `libboost-regex-dev` `libboost-system-dev` `libboost-thread-dev`
  * `libyaml-cpp-dev`
  * `libjpeg-dev`
  * `libboost-test-dev` (_required only for tests_).

The whole project has been built and tested using:

```
#!bash
$ g++ --version
g++ (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
```
```
#!bash
$ ar --version
GNU ar (GNU Binutils for Ubuntu) 2.23.52.20130913
```
```
#!bash
$ dpkg -s libboost-dev | grep "Version"
Version: 1.53.0.0ubuntu2
```
```
#!bash
$ dpkg -s libyaml-cpp-dev | grep "Version"
Version: 0.3.0-1
```
```
#!bash
$ dpkg -s libjpeg-dev | grep "Version"
Version: 8c-2ubuntu8
```


Running
-------

After building the project, all programs are placed in the `bin/` folder. It also contains many scripts which can be used to launch the executables without manually providing all necessary arguments (IP addresses and ports):

    :::bash
    bin/run-registry
    bin/run-storageprovider
    bin/run-imagemanipulationprovider
    bin/run-client1
    bin/run-client2
    bin/run-client3
    bin/run-client4
    bin/run-client5

The executable `bin/testcase` can be used to launch all programs together, showing their outputs in the same terminal: each program will be associated with a different color.


How to use the library in a client
==================================

The following listing shows how can be structured the source code of a generic client which uses the “StoreImage” service.

```
#!cpp
// main.cpp

// Include library headers
#include <ssoa/registry/registry.h>
#include <ssoa/utils.h>
// Include headers distributed by the service provider
#include <storageprovider/storeimageservice.h>

using namespace ssoa;

int main(int argc, char *argv[])
{
    string registryAddress, registryPort;

    // Parse command line options
    // ...

    // Initialize the library
    ssoa::setup();

    // Initialize the local stub for the registry
    Registry::initialize(registryAddress, registryPort);

    string name = <filename>;
    vector<byte> buffer = readWholeFile(name);

    string signature = StoreImageService::serviceSignature();

    // Ask the registry for the address and port of the service provider
    pair<string, string> hostAndPort = Registry::getProvider(signature);

    // Create an instance of the stub
    StoreImageService storeImage(hostAndPort.first, hostAndPort.second);

    // Use the service
    if (!storeImage.invoke(name, buffer)) {
        cerr << "Cannot store image on server." << endl;
        cerr << "  Returned status: " << storeImage.getStatus() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```


How to implement a service provider
===================================

The library provides a set of classes which help writing new service providers. For instance, a `ServiceListener` can be used to listen on a given port, accept incoming connections and dispatch them according to the requested service. To implement a new service provider, the following steps are required for _each_ provided service:

  * The provider _must_ implement the _skeleton_ of the service, that is a class which extends `ssoa::ServiceSkeleton`. This class, given all input arguments (already deserialized), will execute the business logic in order to service the client's request and produce a `Response` which will be sent (by the library) back to the client.
  * The provider _should_ implement the _stub_ of the service, that is a small class (usually a single `.h` or `.hpp` file) which extends `ssoa::ServiceStub`. This class is intended to be made available to all clients.

For example, the following listings show (a simplified version of) the _skeleton_ and the _stub_ for the “StoreImage” service and the structure of the `main` of the provider.

The stub distributed by the provider to clients (a `.hpp` file) has basically just to implement the `invoke()` method.

```
#!cpp
// storeimageservice.hpp

// Service stub
class StoreImageService: public ssoa::ServiceStub
{
public:
    // Provide a simple and natural interface to the client
    bool invoke(std::string name, std::vector<byte> buffer)
    {
        // Generate input arguments for the service provider
        pushArgument(new ssoa::ServiceStringArgument(name));
        pushArgument(new ssoa::ServiceBufferArgument(buffer));

        // Submit the request and wait for a Response
        std::unique_ptr<ssoa::Response> response(ssoa::ServiceStub::submit());
        status = response->getStatus();
        return response->isSuccessful();
    }

    // ...
};
```

Among the provider's source files there is the skeleton implementation and the `main`.

```
#!cpp
// storeimageserviceimpl.h

// Service skeleton
class StoreImageServiceImpl: public ssoa::ServiceSkeleton
{
public:
    static const char * serviceSignature() {
        return "StoreImage(in string, in buffer)";
    }

    // Static method invoked on deserialization
    static ssoa::ServiceSkeleton * create(arg_deque&& arguments) {
        StoreImageServiceImpl * s = new StoreImageServiceImpl(...);
        // Build an instance using given input arguments (already deserialized)
        return s;
    }

    // Install a factory for the service so that it can be instantiated on request
    static void install() {
        factory().install(serviceSignature(), create);
    }

    // Implement the service
    virtual ssoa::Response * invoke();
};
```

```
#!cpp
// storeimageserviceimpl.cpp

Response * StoreImageServiceImpl::invoke()
{
    // Read input arguments
    unique_ptr<ServiceStringArgument> name(popArgument<ServiceStringArgument>());
    unique_ptr<ServiceBufferArgument> buffer(popArgument<ServiceBufferArgument>());

    // Execute all operations needed to actually implement the service
    StorageService::saveFile(name->getValue(), buffer->getValue());

    // Produce an appropriate Response
    return new Response(serviceSignature(), true, "OK");
}
```

```
#!cpp
// main.cpp

#include <storeimageserviceimpl.h>
#include <ssoa/registry/registry.h>
#include <ssoa/service/servicelistener.h>
#include <ssoa/utils.h>
// ...

using namespace ssoa;
// ...

int main(int argc, char* argv[])
{
    string localAddress, localPort;
    string registryAddress, registryPort;
    int numThreads;
    int status = 0;

    // Parse command line options
    // ...

    // Initialize the library
    ssoa::setup();

    // Initialize the local stub for the registry
    Registry::initialize(registryAddress, registryPort);

    // Install on the local dispatcher the skeleton of each service
    StoreImageServiceImpl::install();
    // ...

    // Register all services on the registry
    Registry::registerService(StoreImageServiceImpl::serviceSignature(),
        localAddress, localPort);
    // ...

    // Initialize and execute the listener
    ServiceListener server(localAddress, localPort, numThreads);
    server.run();

    // Deregister all services from the registry
    Registry::deregisterService(StoreImageServiceImpl::serviceSignature(),
        localAddress, localPort);
    // ...

    return status;
}
```


How to implement a registry
===========================

The library provides a set of classes to quickly implement a registry. In particular, the `RegistryListener` can be used to listen on a specific port, accept incoming connections and forward the requested operations to a given implementation of the registry. To implement a registry server:

  * Write a class which implements the interface `ssoa::IRegistry`. This class will take care of executing all operations related to the registry, such as binding a provider to a service, removing it, or finding the address of a provider for a specific service.

The following listing shows the required source code.

```
#!cpp
// Initialize the library
ssoa::setup();

// Create an instance of the actual registry implementation
RegistryImpl registry;

// Initialize and execute the listener
ssoa::RegistryListener server(address, port, numThreads, registry);
server.run();
```


Implementation details
======================


Communication protocol with the registry
----------------------------------------

Messages exchanged with the registry are of five types:

  * `registration-request`: used by a provider to register itself for a certain service;
  * `registration-response`: sent by the registry to a provider in response to a `registration-request`;
  * `service-request`: used by a client to obtain the address of any provider for a given service;
  * `service-response`: sent by the registry to a client in response to a `service-request`;
  * `error`: used as a response to notify error conditions (e.g., malformed YAML code, internal server error).


### Messages exchanged with service providers

Upon receiving a registration request such as:

    :::yaml
    type: registration-request
    service: HorizontalFlipImage (in buffer, out buffer)
    host: 131.114.9.35
    port: 1235

if the operation succeeds the registry sends back a response like the following:

    :::yaml
    type: registration-response
    successful: true

or like the following in case of failure:

    :::yaml
    type: registration-response
    succesful: false
    status: Service already registered with different signature.

If the operation is unsuccessful, the registry _should_ inform the provider about the reason by using the `status` field of the response. As a response to a registration request, moreover, the registry can send an `error` message if the `registration-response` cannot be generated, and the provider _must_ be capable to receive and correctly decode it.

When a service provider wants to deregister itselt from a service, it sends to the registry a message in the following form:

    :::yaml
    type: registration-request
    service: HorizontalFlipImage (in buffer, out buffer)
    host: 131.114.9.35
    port: 1235
    unregister: true

If a provider wants to remove all services it provides on a specific port, it can send to the registry a message like the following:

    :::yaml
    type: registration-request
    service: *
    host: 131.114.9.35
    port: 1235
    unregister: true


### Messages exchanged with clients

A tipical request sent by a client to the registry is like the following:

    :::yaml
    type: service-request
    service: RotateImage (in int, in buffer, out buffer)

The associated response is something like:

    :::yaml
    type: service-response
    succesful: true
    host: 131.114.9.35
    port: 1234

or, in case of failure:

    :::yaml
    type: service-response
    succesful: false
    status: No provider available for the requested service.

If the operation is unsuccessful, the registry _should_ inform the client about the reason by using the `status` field of the response. As a response to a service request, moreover, the registry can send an `error` message if the `service-response` cannot be generated, and the client _must_ be capable to receive and correctly decode it.


Communication protocol between a client and a provider
------------------------------------------------------

Both request and response messages exchanged between a client and a provider are composed by a null-terminated header string and an optional data payload, in which are stored all arguments (if present):

    <header>\0<payload>

For a request message, the header has the following form:

    :::yaml
    service: RotateImage (in int, in buffer, out buffer)
    blocks: [ 4, 403912 ]

It contains the signature of the requested service and an array with the sizes of all input arguments sent in the payload.

In the context of a response, instead, the array lists the sizes of all output argument sent in the payload. Moreover, other fields are present which carry information about the result of the operation:

    :::yaml
    service: RotateImage (in int, in buffer, out buffer)
    succesful: true
    status: OK
    blocks: [ 560231 ]

In case of failure, the whole payload can be omitted and the `blocks` field left empty:

    :::yaml
    service: RotateImage (in int, in buffer, out buffer)
    succesful: false
    status: Image format not supported.
    blocks: [ ]
