/**
 * main.cpp
 */

#include <ssoa/registry/registry.h>
#include <ssoa/utils.h>
#include <storageprovider/storeimageservice.h>
#include <storageprovider/getimageservice.h>
#include <storageprovider/getlistservice.h>
#include <imagemanipulationprovider/rotateimageservice.h>
#include <imagemanipulationprovider/horizontalflipimageservice.h>

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include <netinet/in.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace std;
using namespace ssoa;
using namespace storageprovider;
using namespace imagemanipulationprovider;
namespace po = boost::program_options;

typedef unsigned char byte;

string imageFolder;

bool readRandomFileFromDisk(string& filename, vector<byte>& buffer)
{
    vector<string> files;
    for (boost::filesystem::recursive_directory_iterator it(imageFolder), end; it != end; ++it) {
        if (boost::filesystem::is_regular_file(it->status())
            && boost::iequals(it->path().extension().c_str(), ".jpg")) {
            files.push_back(it->path().c_str());
        }
    }
    if (files.size() == 0) {
        Logger::error("No image found in folder '%1%'.", imageFolder);
        return false;
    }

    // In argument filename skip the path of imageFolder
    boost::filesystem::path combinedPath(files[rand() % files.size()]), relativePath;
    boost::filesystem::path skipPath(imageFolder);
    skipPath.normalize();
    auto itCombined = combinedPath.begin();
    for (auto itSkip = skipPath.begin(); itSkip != skipPath.end();) {
        if (*itSkip == *itCombined) {
            ++itSkip;
            ++itCombined;
            continue;
        }
        if (itSkip->string() == ".")
            ++itSkip;
        else if (itCombined->string() == ".")
            ++itCombined;
        else
            break; // Something went wrong
    }
    while (itCombined != combinedPath.end()) {
        relativePath /= *itCombined;
        ++itCombined;
    }
    filename = relativePath.string();

    ifstream ifs(combinedPath.c_str(), ios::binary | ios::ate);
    buffer.resize(ifs.tellg());
    ifs.seekg(0, ios::beg);
    ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    ifs.close();
    return true;
}

bool readRandomFileFromStorageProvider(string& name, vector<byte>& buffer)
{
    pair<string, string> pair = Registry::getProvider(GetListService::serviceSignature());

    GetListService getList(pair.first, pair.second);

    vector<string> list;
    if (!getList.invoke(list)) {
        Logger::error("Cannot retrieve list of images from the server.");
        Logger::error("  Returned status: %1%", getList.getStatus());
        return false;
    }

    if (list.size() == 0) {
        Logger::error("No image available on the storage provider.");
        return false;
    }

    name = list[rand() % list.size()];

    pair = Registry::getProvider(GetImageService::serviceSignature());
    GetImageService getImage(pair.first, pair.second);
    if (!getImage.invoke(name, buffer)) {
        Logger::error("Cannot retrieve image '%1%' from the server.", name);
        Logger::error("  Returned status: %1%", getImage.getStatus());
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    string registryAddress, registryPort;

    po::options_description description("Allowed options");
    description.add_options()
        ("help,h", "Shows this help")
        ("registry-address,A", po::value<string>(&registryAddress)->default_value("127.0.0.1"),
            "Specifies the address of the registry")
        ("registry-port,P", po::value<string>(&registryPort),
            "Specifies the port of the registry")
        ("image-folder,f", po::value<string>(&imageFolder),
            "Specifies the folder containing images")
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

    if (vm.find("registry-port") == vm.end()) {
        cerr << "Registry port not specified!" << endl;
        return EXIT_FAILURE;
    }
    if (vm.find("image-folder") == vm.end()) {
        cerr << "Image folder not specified!" << endl;
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    ssoa::setup();
    Registry::initialize(registryAddress, registryPort);

    try {
        while (true) {
            sleep(1);

            string name;
            std::vector<byte> buffer;
            if (rand() & 1) {
                Logger::info("Retrieving an image from disk...");
                if (!readRandomFileFromDisk(name, buffer)) {
                    continue;
                }
            }
            else {
                Logger::info("Retrieving an image from the storage provider...");
                if (!readRandomFileFromStorageProvider(name, buffer)) {
                    continue;
                }
            }
            Logger::info("Selected image '%1%'.", name);

            if (rand() & 1) {
                string signature(RotateImageService::serviceSignature());
                Logger::info("Requesting service '%1%' to registry...", signature);
                pair<string, string> pair = Registry::getProvider(signature);
                Logger::info("Response received (host: %1%, port: %2%).", pair.first, pair.second);
                RotateImageService rotateImage(pair.first, pair.second);
                int degrees = rand() % 360;
                Logger::info("Rotating image by %1% degrees...", degrees);
                if (!rotateImage.invoke(degrees, buffer, buffer)) {
                    Logger::error("Cannot rotate image on server.");
                    Logger::error("  Returned status: %1%", rotateImage.getStatus());
                    continue;
                }
            }
            else {
                string signature(HorizontalFlipImageService::serviceSignature());
                Logger::info("Requesting service '%1%' to registry...", signature);
                pair<string, string> pair = Registry::getProvider(signature);
                Logger::info("Response received (host: %1%, port: %2%).", pair.first, pair.second);
                HorizontalFlipImageService hflipImage(pair.first, pair.second);
                Logger::info("Flipping image horizontally...");
                if (!hflipImage.invoke(buffer, buffer)) {
                    Logger::error("Cannot flip image on server.");
                    Logger::error("  Returned status: %1%", hflipImage.getStatus());
                    continue;
                }
            }

            string signature(StoreImageService::serviceSignature());
            Logger::info("Requesting service '%1%' to registry...", signature);
            pair<string, string> pair = Registry::getProvider(signature);
            Logger::info("Response received (host: %1%, port: %2%).", pair.first, pair.second);
            StoreImageService storeImage(pair.first, pair.second);
            Logger::info("Storing image on server...");
            if (!storeImage.invoke(name, buffer)) {
                Logger::error("Cannot store image on server.");
                Logger::error("  Returned status: %1%", storeImage.getStatus());
                continue;
            }
        };
    }
    catch (const std::exception& e) {
        Logger::error("Caught an exception: %1%", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
