/*
 * imagehelper.cpp
 */

#include <imagehelper.h>

#include <fstream>
#include <stdexcept>
#include <vector>

#include <boost/filesystem.hpp>

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

#define cimg_use_jpeg
#include <CImg.h>
using cimg_library::CImg;

namespace imagemanipulationprovider
{
    static void loadFile(string filename, vector<unsigned char>& buffer)
    {
        ifstream infile(filename.c_str(), ifstream::binary | ifstream::in | ifstream::ate);
        buffer.resize(infile.tellg());
        infile.seekg(0, ifstream::beg);
        infile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        infile.close();
        if (infile.fail()) {
            throw std::runtime_error("Cannot read file '" + filename + "' (" + strerror(errno) + ").");
        }
    }

    static void saveFile(string filename, const vector<unsigned char>& buffer)
    {
        ofstream outfile(filename.c_str(), ofstream::binary | ofstream::out);
        outfile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        outfile.flush();
        outfile.close();
        if (outfile.fail()) {
            throw std::runtime_error("Cannot write file '" + filename + "' (" + strerror(errno) + ").");
        }
    }

    void ImageHelper::rotate(const vector<byte>& input, vector<byte>& output, float degrees)
    {
        boost::filesystem::path path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
        const char * tempfilename = path.c_str();

        saveFile(tempfilename, input);

        CImg<float> image;
        image = image.load_jpeg(tempfilename);
        image.rotate(degrees);
        image.save_jpeg(tempfilename);

        loadFile(tempfilename, output);
        remove(tempfilename);
    }

    void ImageHelper::flipHorizontally(const vector<byte>& input, vector<byte>& output)
    {
        boost::filesystem::path path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
        const char * tempfilename = path.c_str();

        saveFile(tempfilename, input);

        CImg<float> image;
        image = image.load_jpeg(tempfilename);
        image.mirror('x');
        image.save_jpeg(tempfilename);

        loadFile(tempfilename, output);
        remove(tempfilename);
    }
}
