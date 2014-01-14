/*
 * imagehelper.h
 */

#ifndef _IMAGEHELPER_H_
#define _IMAGEHELPER_H_

#include <string>
#include <vector>

namespace imagemanipulationprovider
{
    /// Provides a few image manipulation routines.
    class ImageHelper
    {
        typedef unsigned char byte;

    public:
        /// Rotates an image by the specified angle.
        static void rotate(const std::vector<byte>& input, std::vector<byte>& output, float degrees);

        /// Flips an image horizontally.
        static void flipHorizontally(const std::vector<byte>& input, std::vector<byte>& output);
    };
}

#endif
