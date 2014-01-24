/**
 * main.cpp
 */

#define LOOPBACK "127.0.0.1"

#define PORT_REGISTRY "1200"
#define PORT_STORAGEPROVIDER "1300"
#define PORT_IMPROVIDER "1400"

#define COLOR_REGISTRY "\033[1;31m"
#define COLOR_STORAGEPROVIDER "\033[1;32m"
#define COLOR_IMPROVIDER "\033[1;34m"
#define COLOR_CLIENT "\033[1;33m"
#define COLOR_DEFAULT "\033[0m"

#include <iostream>
#include <stdexcept>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <ssoa/logger.h>

using std::string;
using ssoa::Logger;

class process
{
public:
    template<typename ... Args>
    process(string file, string port, const char *marker, Args ...args) {
        this->file = file;
        pid = fork();
        if (pid == -1) {
            throw std::runtime_error("In execution of '" + file + "': cannot fork!\n");
        }
        if (pid == 0) {
            setpgrp(); // Prevent propagation of signals from parent to child
            Logger::info("Running: '%1%' on port %2%...", file, port);
            if (execl(file.c_str(), file.c_str(), "--log-marker", marker, args...)) {
                throw std::runtime_error("Cannot run '" + file + "'! Make sure it is in the current folder.\n");
            }
        }
    }

    void kill() {
        if (pid != 0) {
            Logger::info("Terminating process '%1%...", file);
            ::kill(pid, SIGTERM);
        }
    }

    void wait() {
        waitpid(pid, NULL, 0);
        Logger::info("Process '%1%' exited.", file);
    }

    ~process() {
        kill();
        wait();
    }

private:
    int pid;
    string file;
};

class registry: public process
{
public:
    registry(string file, const char *marker, const char *port) :
        process(file, port, marker, "-a", LOOPBACK, "-p", port, NULL)
    {
    }
};

class provider: public process
{
public:
    provider(string file, const char *marker, const char *port, const char *registryPort) :
        process(file, port, marker, "-a", LOOPBACK, "-p", port, "-A", LOOPBACK, "-P", registryPort, NULL)
    {
    }
};

class client: public process
{
public:
    client(string file, const char *marker, const char *registryPort, const char *imageFolder) :
        process(file, "<unspecified>", marker, "-A", LOOPBACK, "-P", registryPort, "-f", imageFolder, NULL)
    {
    }
};

std::vector<process*> children;

void on_kill(int) {
    Logger::info("Request to terminate.");
    for (int i = children.size() - 1; i >= 0; i--) {
        if (children[i]) {
            delete children[i];
            children[i] = NULL;
        }
    }
}

int main()
{
    Logger::marker = COLOR_DEFAULT;
    try {
        children.push_back(new registry("ssoa-registry", COLOR_REGISTRY, PORT_REGISTRY));
        Logger::info("Waiting 1 second...");
        sleep(1);

        children.push_back(
            new provider("ssoa-storageprovider", COLOR_STORAGEPROVIDER, PORT_STORAGEPROVIDER, PORT_REGISTRY));
        Logger::info("Waiting 1 second...");
        sleep(1);

        children.push_back(
            new provider("ssoa-imagemanipulationprovider", COLOR_IMPROVIDER, PORT_IMPROVIDER, PORT_REGISTRY));
        Logger::info("Waiting 1 second...");
        sleep(1);

        for (int i = 1; i <= 5; i++) {
            string marker = string(COLOR_CLIENT) + "[" + string(1, (char)('0' + i)) + "] ";
            string folder = "client" + string(1, (char)('0' + i)) + "images";
            children.push_back(new client("ssoa-client", marker.c_str(), PORT_REGISTRY, folder.c_str()));
            Logger::info("Waiting 1 second...");
        }
        sleep(1);

        signal(SIGINT, on_kill);
        signal(SIGTERM, on_kill);
        signal(SIGQUIT, on_kill);

        Logger::info("Running...");
        while (waitpid(-1, NULL, 0)) {
            if (errno == ECHILD) {
                break;
            }
        }
        Logger::info("All children exited.");
    } catch (const std::exception& e) {
        Logger::error("In testcase, caught an exception: %1%", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
