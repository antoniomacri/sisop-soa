#include <service/service.h>
#include <service/response.h>

#include <boost/asio.hpp>
#include <yaml-cpp/yaml.h>

#include <sstream>
#include <stdexcept>

using std::istringstream;
using std::ostringstream;
using std::string;
using std::vector;

namespace ssoa
{
    void Service::throw_invalid_argument(int index) const
        {
        string expected = signature.getInputParams()[index];
        throw std::logic_error("Invalid argument (must be '" + expected + "').");
    }

    void Service::throw_missing_arguments() const
    {
        ostringstream oss;
        oss << (signature.getInputParams().size() - arguments.size());
        throw std::logic_error("Still missing " + oss.str() + " argument(s).");
    }

    void Service::pushArgument(const int32_t *arg)
    {
        if (signature.getInputParams()[arguments.size()] != string("int"))
            throw_invalid_argument(arguments.size());
        arguments.push_back(Argument(arg));
    }

    void Service::pushArgument(const double *arg)
    {
        if (signature.getInputParams()[arguments.size()] != string("double"))
            throw_invalid_argument(arguments.size());
        arguments.push_back(Argument(arg));
    }

    void Service::pushArgument(const string *arg)
    {
        if (signature.getInputParams()[arguments.size()] != string("string"))
            throw_invalid_argument(arguments.size());
        arguments.push_back(Argument(arg));
    }

    void Service::pushArgument(const std::vector<byte> *arg)
    {
        if (signature.getInputParams()[arguments.size()] != string("buffer"))
            throw_invalid_argument(arguments.size());
        arguments.push_back(Argument(arg));
    }

    void Service::popArgument(int32_t *arg)
    {
        Argument& a = arguments.front();
        if (a.type != string("int"))
            throw_invalid_argument(0);
        *arg = *((int*)a.value);
        arguments.pop_front();
    }

    void Service::popArgument(double *arg)
    {
        Argument& a = arguments.front();
        if (a.type != string("double"))
            throw_invalid_argument(0);
        *arg = *((double*)a.value);
        arguments.pop_front();
    }

    void Service::popArgument(string *arg)
    {
        Argument& a = arguments.front();
        if (a.type != string("string"))
            throw_invalid_argument(0);
        *arg = *((string*)a.value);
        arguments.pop_front();
    }

    void Service::popArgument(std::vector<byte> *arg)
    {
        Argument& a = arguments.front();
        if (a.type != string("buffer"))
            throw_invalid_argument(0);
        *arg = *((vector<byte>*)a.value);
        arguments.pop_front();
    }

    Response * Service::submit()
    {
        if (arguments.size() < signature.getInputParams().size())
            throw_missing_arguments();

        boost::asio::ip::tcp::iostream s(host, port);

        // Construct and send the YAML header
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "service" << YAML::Value << signature;
        e << YAML::Key << "blocks";
        e << YAML::BeginSeq;
        for (unsigned i = 0; i < arguments.size(); i++) {
            e << arguments[i].size;
        }
        e << YAML::EndSeq;
        e << YAML::EndMap;
        s << string(e.c_str());

        // Send the payload with data blocks
        for (unsigned i = 0; i < arguments.size(); i++) {
            s.write((const char*)arguments[i].value, arguments[i].size);
        }

        return Response::fromStream(s);
    }

    Service * Service::fromStream(std::istream stream)
    {
        string yaml;
        stream >> yaml;
        istringstream ss(yaml);
        YAML::Parser parser(ss);
        YAML::Node node;
        parser.GetNextDocument(node);

        ServiceSignature signature = node["service"].to<string>();

        // TODO: we could add a callback method to validate the signature
        // and check if the provider actually supports the service,
        // in order to avoid storing all arguments when the service is unavailable
        // (we must always gobble them, of course)

        int block;
        std::vector<int> blocks;
        const YAML::Node & blocksNode = node["blocks"];
        for (unsigned i = 0; i < blocksNode.size(); i++) {
            blocksNode[i] >> block;
            blocks.push_back(block);
        }

        Service *service = new Service(signature);
        const vector<string> & params = signature.getInputParams();
        for (unsigned i = 0; i < params.size(); i++) {
            void *buffer, *object;
            /*
            // TODO: deserialize service arguments
            switch (params[i]) {
                case ServiceSignature::TYPE_INT:
                    // TODO: should we check if block[i]==sizeof(int32_t)?
                    object = buffer = new int;
                    break;
                case ServiceSignature::TYPE_DOUBLE:
                    // TODO: should we check if block[i]==sizeof(double)?
                    object = buffer = new double;
                    break;
                case ServiceSignature::TYPE_STRING: {
                    string *s = new string();
                    s->resize(blocks[i]);
                    object = s;
                    buffer = (void*)s->data();
                    break;
                }
                case ServiceSignature::TYPE_BUFFER: {
                    vector<byte> *v = new vector<byte>();
                    v->resize(blocks[i]);
                    object = v;
                    buffer = (void*)v->data();
                    break;
                }
                case ServiceSignature::TYPE_INVALID:
                    break;
            }
            */
            stream.read((char*)buffer, blocks[i]);
            // TODO: check if gcount()==blocks[i]
            service->arguments.push_back(Argument(object, params[i], blocks[i]));
        }
        return service;
    }
}
