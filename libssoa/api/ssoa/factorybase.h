/*
 * factorybase.h
 */

#ifndef _FACTORYBASE_H_
#define _FACTORYBASE_H_

#include <ssoa/logger.h>

#include <map>
#include <string>
#include <utility>

namespace ssoa
{
    /// Support for synchronous serialization of a class hierarchy.
    ///
    /// Methods of this class should never be called from inside the destructor of a static
    /// object.
    template<class T, typename ... Args>
    class FactoryBase
    {
    public:
        FactoryBase(std::string hierarchyName) :
            hierarchyName(hierarchyName)
        {
        }

        /// Represents a method which generates an instance of class @c T from the given @c args.
        ///
        /// This method should not return @c NULL. If the message cannot be constructed,
        /// an exception @c std::runtime_error should be thrown.
        typedef T * (*CreatorMethod)(Args ... args);

        /// Checks if a handler for the specified class is installed.
        ///
        /// @param className A null-terminated string identifying the class.
        bool contains(const std::string className)
        {
            return mappings().find(className) != mappings().end();
        }

        /// Installs the creation handler for the specified class belonging to the hierarchy of @c T.
        ///
        /// @param className A null-terminated string identifying the class.
        /// @param creator A function which handles the creation of the new instance.
        ///
        /// @throws std::logic_error A handler is already registered for the given class.
        ///
        /// @see CreatorMethod
        void install(const std::string className, CreatorMethod creator)
        {
            auto iter = mappings().find(className);
            if (iter != mappings().end()) {
                throw std::logic_error(hierarchyName +
                                       std::string(": Duplicate initialization for identifier \"")
                                       + className + "\".");
            }
            std::string s("FactoryBase: in hierarchy '" + hierarchyName +
                          "' added factory method for '" + className + "'.");
            Logger::debug() << s << std::endl;
            mappings()[className] = creator;
        }

        /// Constructs a new instance of the proper class in the @c T hierarchy from the given @c args.
        ///
        /// @param className A null-terminated string identifying the class.
        /// @param args  The list of arguments provided to the installed handler.
        ///
        /// @return The newly created instance of the class belonging to the @c T hierarchy.
        ///         The return value is never @c NULL.
        ///
        /// @throws std::runtime_error No handler is installed for the specified class.
        T * create(const std::string className, Args ... args)
        {
            auto iter = mappings().find(className);
            if (iter == mappings().end()) {
                throw std::runtime_error(hierarchyName +
                                         std::string(": Unknown message type: \"")
                                         + className + "\".");
            }
            return (iter->second)(std::forward<Args>(args)...);
        }

    private:
        std::string hierarchyName;

        /// Contains mappings between a class and its "named constructor"
        std::map<std::string, CreatorMethod>& mappings()
        {
            // Construct-on-first-use to avoid the static initialization order problem
            static std::map<std::string, CreatorMethod> m;
            // Using static: many instances with equal template arguments will share the same m;
            // Using static object instead of static pointer: be careful not to call this method
            // from the destructor of a static object.
            return m;
        }
    };
}

#ifndef stringify
#define stringify(x) #x
#endif

#endif
