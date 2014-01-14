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

using namespace std;
using namespace ssoa;
using namespace storageprovider;
using namespace imagemanipulationprovider;

typedef unsigned char byte;

string imageFolder = "images";

bool readRandomFileFromDisk(string& filename, vector<byte>& buffer)
{
    boost::filesystem::path p(imageFolder);
    vector<string> files;
    for (boost::filesystem::directory_iterator it(p), end; it != end; ++it) {
        if (boost::filesystem::is_regular_file(it->status())
            && boost::iequals(it->path().extension().c_str(), ".jpg")) {
            files.push_back(it->path().c_str());
        }
    }
    if (files.size() == 0) {
        cerr << "No image found in folder \"" << imageFolder << "\"." << endl;
        return false;
    }
    filename = files[rand() % files.size()];
    ifstream ifs(filename.c_str(), ios::binary | ios::ate);
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
        cerr << "Cannot retrieve list of images from the server." << endl;
        cerr << "  Returned status: " << getList.getStatus() << endl;
        return false;
    }

    if (list.size() == 0) {
        cerr << "No image available on the storage provider." << endl;
        return false;
    }

    name = list[rand() % list.size()];

    pair = Registry::getProvider(GetImageService::serviceSignature());
    GetImageService getImage(pair.first, pair.second);
    if (!getImage.invoke(name, buffer)) {
        cerr << "Cannot retrieve image \"" << name << "\" from the server." << endl;
        cerr << "  Returned status: " << getImage.getStatus() << endl;
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <address> <port>" << std::endl;
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    ssoa::setup();
    Registry::initialize(argv[1], argv[2]);

    try {
        while (true) {
            cout << endl;
            sleep(1);

            string name;
            std::vector<byte> buffer;
            if (rand() & 1) {
                cout << "Retrieving an image from disk..." << endl;
                if (!readRandomFileFromDisk(name, buffer)) {
                    continue;
                }
            }
            else {
                cout << "Retrieving an image from the storage provider..." << endl;
                if (!readRandomFileFromStorageProvider(name, buffer)) {
                    continue;
                }
            }
            cout << "Selected image '" << name << "'." << endl;

            if (rand() & 1) {
                string signature(RotateImageService::serviceSignature());
                cout << "Requesting service \"" << signature << "\" to registry..." << endl;
                pair<string, string> pair = Registry::getProvider(signature);
                cout << "Response received (host: " << pair.first << ", port: " << pair.second << ")." << endl;
                RotateImageService rotateImage(pair.first, pair.second);
                int degrees = rand() % 360;
                cout << "Rotating image by " << degrees << " degrees..." << endl;
                if (!rotateImage.invoke(degrees, buffer, buffer)) {
                    cerr << "Cannot rotate image on server." << endl;
                    cerr << "  Returned status: " << rotateImage.getStatus() << endl;
                    continue;
                }
            }
            else {
                string signature(HorizontalFlipImageService::serviceSignature());
                cout << "Requesting service \"" << signature << "\" to registry..." << endl;
                pair<string, string> pair = Registry::getProvider(signature);
                cout << "Response received (host: " << pair.first << ", port: " << pair.second << ")." << endl;
                HorizontalFlipImageService hflipImage(pair.first, pair.second);
                cout << "Flipping image horizontally..." << endl;
                if (!hflipImage.invoke(buffer, buffer)) {
                    cerr << "Cannot flip image on server." << endl;
                    cerr << "  Returned status: " << hflipImage.getStatus() << endl;
                    continue;
                }
            }

            string signature(StoreImageService::serviceSignature());
            cout << "Requesting service \"" << signature << "\" to registry..." << endl;
            pair<string, string> pair = Registry::getProvider(signature);
            cout << "Response received (host: " << pair.first << ", port: " << pair.second << ")." << endl;
            StoreImageService storeImage(pair.first, pair.second);
            cout << "Storing image on server..." << endl;
            if (!storeImage.invoke(name, buffer)) {
                cerr << "Cannot store image on server." << endl;
                cerr << "  Returned status: " << storeImage.getStatus() << endl;
                continue;
            }
        };
    }
    catch (const std::exception& e) {
        cerr << "Caught an exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
