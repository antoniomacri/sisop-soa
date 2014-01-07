/**
 *  main.cpp
 */

#include <registrylistener.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <logger.h>

using namespace std;
using namespace ssoa;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    string address, port;
    int num_threads;

    po::options_description description("Allowed options");
    description.add_options()
        ("help", "Shows this help")
        ("address,a", po::value<string>(&address), "Listens on the given local address")
        ("port,p", po::value<string>(&port), "Listens on the given local port")
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

    if (vm.find("address") == vm.end()) {
        Logger::info() << "Local address not specified: registry will listen on the loopback interface." << endl;
    }

    try {
        // Initialize and run the server until stopped.
        ssoa::RegistryListener server(address, port, num_threads);
        server.run();
    }
    catch (const exception& e) {
        Logger::error() << "Caught an exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
