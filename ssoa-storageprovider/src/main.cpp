/**
 * main.cpp
 */

#include <storeimageserviceimpl.h>
#include <getimageserviceimpl.h>
#include <getlistserviceimpl.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <ssoa/logger.h>
#include <ssoa/registry/registry.h>
#include <ssoa/service/servicelistener.h>
#include <ssoa/utils.h>

using namespace std;
using namespace ssoa;
using namespace storageprovider;
namespace po = boost::program_options;

template<class T>
void registerService(const string& address, const string& port)
{
    T::install();
    Logger::info("Installed service '%1%'.", T::serviceSignature());
    Registry::registerService(T::serviceSignature(), address, port);
    Logger::info("Registered service '%1%' on the registry.", T::serviceSignature());
}

template<class T>
void deregisterService(const string& address, const string& port)
{
    Registry::deregisterService(T::serviceSignature(), address, port);
    Logger::info("Deregistered service '%1%' from the registry.", T::serviceSignature());
}

int main(int argc, char* argv[])
{
    string address, port;
    string registryAddress, registryPort;
    int num_threads;

    po::options_description description("Allowed options");
    description.add_options()
        ("help,h", "Shows this help")
        ("address,a", po::value<string>(&address)->default_value("127.0.0.1"),
            "Listens on the given local address")
        ("port,p", po::value<string>(&port),
            "Listens on the given local port")
        ("registry-address,A", po::value<string>(&registryAddress)->default_value("127.0.0.1"),
            "Specifies the address of the registry")
        ("registry-port,P", po::value<string>(&registryPort),
            "Specifies the port of the registry")
        ("threads,n", po::value<int>(&num_threads)->default_value(10),
            "Specifies the number of threads in the pool")
        ("log-marker,l", po::value<string>(&Logger::marker),
            "Specifies a string printed at the beginning of every log message");

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, description), vm);
        po::notify(vm);
    }
    catch (const exception& e) {
        cerr << "Error parsing command line arguments: " << e.what() << endl;
        cerr << description;
        return EXIT_FAILURE;
    }

    if (vm.find("help") != vm.end()) {
        cout << description;
        return EXIT_FAILURE;
    }

    if (vm.find("port") == vm.end()) {
        cerr << "Local port not specified!" << endl;
        return EXIT_FAILURE;
    }
    if (vm.find("registry-port") == vm.end()) {
        cerr << "Registry port not specified!" << endl;
        return EXIT_FAILURE;
    }

    // Initialize the library
    ssoa::setup();

    Registry::initialize(registryAddress, registryPort);
    Logger::info("Initialized registry as %1%:%2%.", registryAddress, registryPort);

    try {
        registerService<StoreImageServiceImpl>(address, port);
        registerService<GetImageServiceImpl>(address, port);
        registerService<GetListServiceImpl>(address, port);
    }
    catch (const exception& e) {
        Logger::error("Exception while registering services: %1%", e.what());
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;
    try {
        // Initialize and run the server until stopped.
        ServiceListener server(address, port, num_threads);
        Logger::info("Server started.");
        server.run();
    }
    catch (const exception& e) {
        Logger::error("Exception while running server: %1%", e.what());
        // Do not return: deregister services.
        status = EXIT_FAILURE;
    }

    try {
        deregisterService<StoreImageServiceImpl>(address, port);
        deregisterService<GetImageServiceImpl>(address, port);
        deregisterService<GetListServiceImpl>(address, port);
    }
    catch (const exception& e) {
        Logger::error("Exception while deregistering services: %1%", e.what());
        return EXIT_FAILURE;
    }

    return status;
}
