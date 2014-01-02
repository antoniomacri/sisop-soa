/*
 * response.h
 */

#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <iostream>
#include <string>
#include <vector>

namespace ssoa
{
    typedef unsigned char byte;

    class Response
    {
    public:
        Response();

        void getArgument(int *arg);
        void getArgument(double *arg);
        void getArgument(std::string *arg);
        void getArgument(std::vector<byte> *arg);

        static Response * fromStream(std::iostream& stream);
    };
}

#endif
