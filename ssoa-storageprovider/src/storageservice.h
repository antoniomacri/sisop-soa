/*
 * storageservice.h
 */

#ifndef _STORAGESERVICE_H_
#define _STORAGESERVICE_H_

#include <string>
#include <vector>

#include <boost/thread/shared_mutex.hpp>

namespace storageprovider
{
    /// Reads and writes files from/to a specific folder.
    class StorageService
    {
    public:
        /// Sets the path of the folder where all files are stored.
        static void setPath(std::string path);

        /// Gets the list of all files.
        static std::vector<std::string> getList();

        /// Loads the file with the given file name.
        static void loadFile(std::string filename, std::vector<unsigned char>& buffer);

        /// Saves a file with the given filename.
        static void saveFile(std::string filename, const std::vector<unsigned char>& buffer);

    private:
        StorageService() {
        }

        static boost::shared_mutex mutex;
        static std::string path;
    };
}

#endif
