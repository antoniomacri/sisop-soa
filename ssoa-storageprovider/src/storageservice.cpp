/*
 * storageservice.cpp
 */

#include "storageservice.h"

#include <fstream>

#include <boost/filesystem.hpp>

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using boost::shared_mutex;
using boost::shared_lock;
using boost::unique_lock;

namespace storageprovider
{
    string StorageService::path = "database";
    shared_mutex StorageService::mutex;

    void StorageService::setPath(string path)
    {
        unique_lock<shared_mutex> writerLock(mutex);
        StorageService::path = path;
    }

    vector<string> StorageService::getList()
    {
        shared_lock<shared_mutex> readerLock(mutex);

        vector<string> files;

        if (!boost::filesystem::exists(path)) {
            return files;
        }

        for (boost::filesystem::recursive_directory_iterator it(path), end; it != end; ++it) {
            if (boost::filesystem::is_regular_file(it->status())) {
                // In returned filenames skip the base path
                boost::filesystem::path relativePath, skipPath(path);
                skipPath.normalize();
                auto itCombined = it->path().begin();
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
                while (itCombined != it->path().end()) {
                    relativePath /= *itCombined;
                    ++itCombined;
                }
                files.push_back(relativePath.string());
            }
        }
        return files;
    }

    void StorageService::loadFile(string filename, vector<unsigned char>& buffer)
    {
        shared_lock<shared_mutex> readerLock(mutex);

        boost::filesystem::path fullPath(path);
        fullPath /= filename;

        if (!boost::filesystem::exists(fullPath)) {
            throw std::runtime_error("The specified file '" + string(fullPath.c_str()) + "' does not exist.");
        }

        ifstream infile(fullPath.c_str(), ifstream::binary | ifstream::in | ifstream::ate);
        buffer.resize(infile.tellg());
        infile.seekg(0, ifstream::beg);
        infile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        infile.close();
        if (infile.fail()) {
            throw std::runtime_error("Cannot read file '" + string(fullPath.c_str()) + "' (" + strerror(errno) + ").");
        }
    }

    void StorageService::saveFile(string filename, const vector<unsigned char>& buffer)
    {
        unique_lock<shared_mutex> writerLock(mutex);

        boost::filesystem::path fullPath(path);
        fullPath /= filename;

        if (!boost::filesystem::exists(fullPath.parent_path())) {
            if (!boost::filesystem::create_directory(fullPath.parent_path()))
                throw std::runtime_error("Cannot create folder '" + string(fullPath.parent_path().c_str()) + "'.");
        }

        ofstream outfile(fullPath.c_str(), ofstream::binary | ofstream::out);
        outfile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        outfile.flush();
        outfile.close();
        if (outfile.fail()) {
            throw std::runtime_error("Cannot write file '" + string(fullPath.c_str()) + "' (" + strerror(errno) + ").");
        }
    }
}
