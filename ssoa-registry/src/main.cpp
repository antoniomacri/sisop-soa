#include <iostream>
#include <boost/lexical_cast.hpp>

#include <registrylistener.h>

using namespace std;

int main(int argc, char* argv[])
{
    try {
        // Check command line arguments.
        if (argc != 4) {
            cerr << "Usage: http_server <address> <port> <threads>" << endl;
            cerr << "  For IPv4, try:" << endl;
            cerr << "    receiver 0.0.0.0 80 1" << endl;
            cerr << "  For IPv6, try:" << endl;
            cerr << "    receiver 0::0 80 1" << endl;
            return EXIT_FAILURE;
        }

        // Initialize the server.
        size_t num_threads = boost::lexical_cast<size_t>(argv[3]);
        ssoa::RegistryListener server(argv[1], argv[2], num_threads);

        // Run the server until stopped.
        server.run();
    }
    catch (exception& e) {
        cerr << "exception: " << e.what() << "\n";
    }

    return EXIT_SUCCESS;
}
