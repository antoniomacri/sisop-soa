/**
 * @file logger.h
 *
 * @author ...
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif

#include <iostream>

namespace ssoa
{
    /**
     * Specifies priority levels for Logger. The items are in ascending priority.
     * If a message has a priority lower than LOG_LEVEL is discarded.
     *
     * @see Makefile for LOG_LEVEL definition
     */
    enum LogLevel
    {
        LOG_DEBUG, /**< Debug and other needless infos */
        LOG_INFO, /**< Notice infos */
        LOG_ERROR /**< Error condition infos */
    };

    /**
     * Simple logger utility.
     * Writes stuff to std::clog stream, according to their priority.
     * Log entries are in this form:
     *
     * @verbatim [timestamp] [level info] ... message ... @endverbatim
     *
     * Priority levels are defined in LogLevel.
     * Even if operations on POSIX streams should be atomic, this class is
     * NOT thread-safe.
     */
    class Logger
    {
    public:
        /** Writes with debug priority */
        static std::ostream& debug();
        /** Writes with info priority */
        static std::ostream& info();
        /** Writes with error priority */
        static std::ostream& error();

    private:
        /** Writes stream */
        static std::ostream& log(LogLevel);
    };

}

#endif
