
#define BOOST_TEST_MODULE registryimpl_test
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#define REGISTRY_ADDRESS "127.0.0.1"
#define REGISTRY_PORT "1200"

#include <ssoa/logger.h>
#include <ssoa/registry/registry.h>
#include <ssoa/utils.h>

#include <signal.h>
#include <unistd.h>

#include <boost/test/unit_test.hpp>

#define BOOST_CHECK_NO_EXCEPTION(a) \
    try { a; } \
    catch (const std::exception& e) { std::cerr << "Unexpected exception: " << e.what() << std::endl; }

using ssoa::Logger;
using ssoa::Registry;
using std::runtime_error;
using std::string;

int pid;

namespace std
{
    std::ostream& operator <<(std::ostream& out, const std::pair<string, string>& rhs) {
        return out << "[" << rhs.first << ", " << rhs.second << "]";
    }
}

bool init_function()
{
    pid = fork();
    if (pid == -1) {
        Logger::error("Cannot fork!");
        return false;
    }
    if (pid == 0) {
        Logger::info("Running registry...");
        if (execl("ssoa-registry", "ssoa-registry", "-a", REGISTRY_ADDRESS, "-p", REGISTRY_PORT, NULL)) {
            Logger::info("Cannot run registry! Make sure ssoa-registry is in current folder.");
            return false;
        }
    }
    int seconds = 1;
    Logger::info("Waiting %1% second(s)...", seconds);
    sleep(seconds);
    Registry::initialize(REGISTRY_ADDRESS, REGISTRY_PORT);
    return true;
}

int main(int argc, char* argv[])
{
    ssoa::setup();

    int result = boost::unit_test::unit_test_main(&init_function, argc, argv);

    Logger::info("Terminating registry...");
    kill(pid, SIGTERM);

    return result;
}

BOOST_AUTO_TEST_CASE( constructors_test )
{
    BOOST_CHECK_NO_EXCEPTION(Registry::registerService("Test(in int, out buffer)", "www.unipi.it", "1300"));
    BOOST_CHECK_NO_EXCEPTION(Registry::registerService("Test(in int, out buffer)", "www.unipi.it", "1301"));
    BOOST_CHECK_NO_EXCEPTION(Registry::registerService("Test(in int, out buffer)", "www.unipi.it", "1302"));

    BOOST_CHECK_NO_EXCEPTION(Registry::registerService("Test(in int, out buffer)", "131.114.21.51", "1300"));
    BOOST_CHECK_NO_EXCEPTION(Registry::registerService("Test(in int, out buffer)", "131.114.21.51", "1301"));
    BOOST_CHECK_NO_EXCEPTION(Registry::registerService("Test(in int, out buffer)", "131.114.21.51", "1300"));
    BOOST_CHECK_NO_EXCEPTION(Registry::registerService("Test(in int, out buffer)", "131.114.21.51", "1301"));

    BOOST_CHECK_THROW(Registry::registerService("Test(in intout buffer)", "www.unipi.it", "1300"), runtime_error);
    BOOST_CHECK_THROW(Registry::registerService("Test(in int)", "www.unipi.it", "1300"), runtime_error);

    BOOST_CHECK_NO_EXCEPTION(Registry::registerService("Test2(in int)", "131.114.21.51", "1400"));

    std::pair<string, string> pair, expected("131.114.21.51", "1400");
    BOOST_CHECK_NO_EXCEPTION(pair = Registry::getProvider("Test2(in int)"));
    BOOST_CHECK_EQUAL(pair, expected);
    BOOST_CHECK_NO_EXCEPTION(pair = Registry::getProvider("Test2(in int)"));
    BOOST_CHECK_EQUAL(pair, expected);

    std::pair<string, string> first, second, third;
    BOOST_CHECK_NO_EXCEPTION(first = Registry::getProvider("Test(in int, out buffer)"));
    BOOST_CHECK_NO_EXCEPTION(second = Registry::getProvider("Test(in int, out buffer)"));
    BOOST_CHECK_NO_EXCEPTION(third = Registry::getProvider("Test(in int, out buffer)"));
    BOOST_CHECK_NO_EXCEPTION(Registry::getProvider("Test(in int, out buffer)"));
    BOOST_CHECK_NO_EXCEPTION(Registry::getProvider("Test(in int, out buffer)"));
    BOOST_CHECK_NO_EXCEPTION(Registry::getProvider("Test(in int, out buffer)"));
    BOOST_CHECK(first != second);
    BOOST_CHECK(first != third);
    BOOST_CHECK(second != third);
}
