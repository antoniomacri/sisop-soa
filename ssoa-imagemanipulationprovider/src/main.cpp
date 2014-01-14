/**
 * main.cpp
 */

#include <rotateimageserviceimpl.h>
#include <horizontalflipimageserviceimpl.h>

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
using namespace imagemanipulationprovider;
namespace po = boost::program_options;

template<class T>
void registerService(const string& address, const string& port)
{
    T::install();
    Logger::info() << "Installed service \"" << T::serviceSignature() << "\"." << endl;
    Registry::registerService(T::serviceSignature(), address, port);
    Logger::info() << "Registered service \"" << T::serviceSignature() << "\" on the registry." << endl;
}

template<class T>
void deregisterService(const string& address, const string& port)
{
    Registry::deregisterService(T::serviceSignature(), address, port);
    Logger::info() << "Deregistered service \"" << T::serviceSignature() << "\" from the registry." << endl;
}

int main(int argc, char* argv[])
{
    string address, port;
    string registryAddress, registryPort;
    int num_threads;

    po::options_description description("Allowed options");
    description.add_options()
        ("help", "Shows this help")
        ("address,a", po::value<string>(&address)->default_value("127.0.0.1"), "Listens on the given local address")
        ("port,p", po::value<string>(&port), "Listens on the given local port")
        ("registry-address,A", po::value<string>(&registryAddress)->default_value("127.0.0.1"), "Specifies the address of the registry")
        ("registry-port,P", po::value<string>(&registryPort), "Specifies the port of the registry")
        ("threads,n", po::value<int>(&num_threads)->default_value(10), "Specifies the number of threads in the pool");

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
    Logger::info() << "Initialized registry as " << registryAddress << ":" << registryPort << "." << endl;

    try {
        registerService<RotateImageServiceImpl>(address, port);
        registerService<HorizontalFlipImageServiceImpl>(address, port);
    }
    catch (const exception& e) {
        Logger::error() << "Exception while registering services: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;
    try {
        // Initialize and run the server until stopped.
        ServiceListener server(address, port, num_threads);
        Logger::info() << "Server started." << endl;
        server.run();
    }
    catch (const exception& e) {
        Logger::error() << "Exception while running server: " << e.what() << endl;
        // Do not return: deregister services.
        status = EXIT_FAILURE;
    }

    try {
        deregisterService<RotateImageServiceImpl>(address, port);
        deregisterService<HorizontalFlipImageServiceImpl>(address, port);
    }
    catch (const exception& e) {
        Logger::error() << "Exception while deregistering services: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return status;
}
