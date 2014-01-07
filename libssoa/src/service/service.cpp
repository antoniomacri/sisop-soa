/*
 * service.cpp
 */

#include <service/service.h>

#include <boost/asio/buffer.hpp>
#include <yaml-cpp/yaml.h>

using std::string;
using std::vector;

namespace ssoa
{
    vector<boost::asio::const_buffer> Service::getConstBuffers() const
    {
        if (arguments.size() < signature.getInputParams().size()) {
            int n = signature.getInputParams().size() - arguments.size();
            throw std::logic_error("Still missing " + boost::lexical_cast<string>(n) + " argument(s).");
        }

        // Construct the YAML header
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "service" << YAML::Value << signature;
        e << YAML::Key << "blocks";
        e << YAML::BeginSeq;
        for (unsigned i = 0; i < arguments.size(); i++) {
            e << boost::asio::buffer_size(arguments[i]->getData());
        }
        e << YAML::EndSeq;
        e << YAML::EndMap;

        vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(string(e.c_str())));

        // Add the payload with data blocks
        for (unsigned i = 0; i < arguments.size(); i++) {
            buffers.push_back(arguments[i]->getData());
        }

        return buffers;
    }
}
