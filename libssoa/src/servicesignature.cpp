/*
 * servicesignature.cpp
 */

#include <servicesignature.h>
#include <enumhelper.h>

#include <boost/regex.hpp>

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
            Type type = EnumHelper::fromString<Type>((*i).str(2).c_str());
            if (!arguments.empty())
                arguments.append(", ");
            arguments.append(direction + " " + EnumHelper::toString(type));
            if (direction == "in")
                inputParams.push_back(type);
            else
                outputParams.push_back(type);
            if (type == TYPE_INVALID) {
                is_valid = false;
            }
            i++;
        }
        this->signature = name + "(" + arguments + ")";
    }

    ServiceSignature::ServiceSignature(const string & name, Type * inParams, Type * outParams)
    {
        this->is_valid = boost::regex_match(name, boost::regex("\\w+"));
        this->name = name;

        string arguments;
        while (*inParams != TYPE_INVALID) {
            if (*inParams > TYPE_LAST) {
                is_valid = false;
            }
            if (arguments.size())
                arguments.append(", ");
            arguments.append(string("in ") + EnumHelper::toString(*inParams));
            inputParams.push_back(*inParams);
            inParams++;
        }
        while (*outParams != TYPE_INVALID) {
            if (*outParams > TYPE_LAST) {
                is_valid = false;
            }
            if (arguments.size())
                arguments.append(", ");
            arguments.append(string("out ") + EnumHelper::toString(*outParams));
            outputParams.push_back(*outParams);
            outParams++;
        }
        if (inputParams.size() == 0 && outputParams.size() == 0)
            is_valid = false;
        this->signature = name + "(" + arguments + ")";
    }
}
