/*
 * logger.cpp
 */

#include <ssoa/logger.h>

#include <time.h>

using namespace std;

namespace ssoa
{
    string Logger::marker;

    ostream& Logger::log(LogLevel level)
    {
        static ostream logstream(clog.rdbuf());
        logstream.rdbuf((level >= LOG_LEVEL) ? clog.rdbuf() : 0);
        return logstream;
    }
}
