/*
 * factorybase.h
 */

#ifndef _FACTORYBASE_H_
#define _FACTORYBASE_H_

#include <map>
#include <string>

namespace ssoa
{
    /// Support for synchronous serialization of a class hierarchy.
    template<class T, typename ... Args>
    class FactoryBase
    {
    private:
        FactoryBase() {
        }

    public:
        /// Represents a method which generates an instance of class @c T from the given @c args.
        ///
        /// This method should not return @c NULL. If the message cannot be constructed,
        /// an exception @c std::runtime_error should be thrown.
        typedef T * (*CreatorMethod)(Args ... args);

        /// Installs the creation handler for the specified class belonging to the hierarchy of @c T.
        ///
        /// @param className A null-terminated string identifying the class.
        /// @param creator A function which handles the creation of the new instance.
        ///
        /// @throws std::logic_error A handler is already registered for the given class.
        ///
        /// @see CreatorMethod
        static void install(const std::string className, CreatorMethod creator)
        {
            auto iter = mappings().find(className);
            if (iter != mappings().end()) {
                throw std::logic_error(T::hierarchyName() +
                                       std::string(": Duplicate initialization for identifier \"")
                                       + className + "\".");
            }
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
        static T * create(const std::string className, Args ... args)
        {
            auto iter = mappings().find(className);
            if (iter == mappings().end()) {
                throw std::runtime_error(T::hierarchyName() +
                                         std::string(": Unknown message type: \"")
                                         + className + "\".");
            }
            return (iter->second)(args...);
        }

    private:
        /// Contains mappings between a class and its "named constructor"
        static std::map<std::string, CreatorMethod>& mappings()
        {
            // Construct-on-first-use to avoid the static initialization order problem
            static std::map<std::string, CreatorMethod> * m = new std::map<std::string, CreatorMethod>();
            return *m;
        }
    };
}

#ifndef stringify
#define stringify(x) #x
#endif

#endif
