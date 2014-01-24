/**
 * logger.h
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif

#include <iostream>
#include <boost/format.hpp>

namespace ssoa
{
    /// Specifies priority levels for Logger. The items are in ascending priority.
    /// If a message has a priority lower than LOG_LEVEL is discarded.
    //
    // @see Makefile for LOG_LEVEL definition
    enum LogLevel
    {
        LOG_DEBUG, ///< Debug messages.
        LOG_INFO,  ///< Generic information.
        LOG_ERROR  ///< Error conditions.
    };

    /// Simple logger.
    ///
    /// Writes log entries to std::clog stream, according to their priority.
    /// Each log entry is in this form:
    /// @verbatim marker[timestamp] [level info] <message> @endverbatim
    ///
    /// Even if operations on POSIX streams should be atomic, this class is
    /// not thread-safe. The only guarantee provided is that the message
    /// string is fully constructed before being sent to the log.
    class Logger
    {
    public:
        /// The 'marker' printed at the beginning of every log message.
        static std::string marker;

        /// Writes with debug priority.
        template<typename String, typename ...Args>
        static void debug(String format, Args ... args) {
            Logger::log(LOG_DEBUG) << head(LOG_DEBUG) + expand(format, std::forward<Args>(args)...) + "\n";
        }

        /// Writes with info priority.
        template<typename String, typename ...Args>
        static void info(String format, Args ... args) {
            Logger::log(LOG_INFO) << head(LOG_INFO) + expand(format, std::forward<Args>(args)...) + "\n";
        }

        /// Writes with error priority.
        template<typename String, typename ...Args>
        static void error(String format, Args ... args) {
            Logger::log(LOG_ERROR) << head(LOG_ERROR) + expand(format, std::forward<Args>(args)...) + "\n";
        }

    private:
        static std::ostream& log(LogLevel);

        static std::string head(LogLevel level) {
            time_t now = time(NULL);
            std::string time_string = ctime(&now);
            std::string level_name;
            switch (level) {
                case LOG_DEBUG:
                    level_name = "debug";
                    break;
                case LOG_INFO:
                    level_name = "info";
                    break;
                case LOG_ERROR:
                    level_name = "error";
                    break;
            }
            time_string.erase(time_string.find('\n', 0), 1);
            return marker + "[" + time_string + "] [" + level_name + "] ";
        }

        static std::string expand(std::string format) {
            return std::move(format);
        }

        template<typename String>
        static std::string expand(String format) {
            return format;
        }

        template<typename String, typename T1>
        static std::string expand(String format, T1 arg1) {
            return std::move((boost::format(format) % arg1).str());
        }

        template<typename String, typename T1, typename T2>
        static std::string expand(String format, T1 arg1, T2 arg2) {
            return std::move((boost::format(format) % arg1 % arg2).str());
        }

        template<typename String, typename T1, typename T2, typename T3>
        static std::string expand(String format, T1 arg1, T2 arg2, T3 arg3) {
            return std::move((boost::format(format) % arg1 % arg2 % arg3).str());
        }

        template<typename String, typename T1, typename T2, typename T3, typename T4>
        static std::string expand(String format, T1 arg1, T2 arg2, T3 arg3, T4 arg4) {
            return std::move((boost::format(format) % arg1 % arg2 % arg3 % arg4).str());
        }

        template<typename String, typename T1, typename T2, typename T3, typename T4, typename T5>
        static std::string expand(String format, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) {
            return std::move((boost::format(format) % arg1 % arg2 % arg3 % arg4 % arg5).str());
        }

        template<typename String, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        static std::string expand(String format, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6) {
            return std::move((boost::format(format) % arg1 % arg2 % arg3 % arg4 % arg5 % arg6).str());
        }
    };
}

#endif
