/*
 * servicesignature.cpp
 */

#include <service/servicesignature.h>

#include <boost/regex.hpp>

using std::string;

namespace ssoa
{
    ServiceSignature ServiceSignature::any;

    string ServiceSignature::sigPattern = "\\s*(\\w+)\\s*\\(\\s*(.+)\\)\\s*";
    string ServiceSignature::argPattern = "\\G(in|out)\\s+(\\w+)\\s*(,\\s*|$)|\\G(.+)";

    ServiceSignature::ServiceSignature(const string & signature)
    {
        using namespace boost;

        regex sigRegex(sigPattern);
        this->is_valid = regex_match(signature, sigRegex);
        this->name = regex_replace(signature, sigRegex, "\\1");

        string arguments;
        string args = regex_replace(signature, sigRegex, "\\2");
        regex argRegex(argPattern);
        sregex_iterator i(args.begin(), args.end(), argRegex), j;
        while (i != j)
        {
            if (!(*i).str(4).empty()) {
                is_valid = false;
                break;
            }
            string direction = (*i).str(1);
            string type = (*i).str(2);
            if (!arguments.empty())
                arguments.append(", ");
            arguments.append(direction + " " + type);
            if (direction == "in")
                inputParams.push_back(type);
            else
                outputParams.push_back(type);
            i++;
        }
        this->signature = name + "(" + arguments + ")";
    }
}
