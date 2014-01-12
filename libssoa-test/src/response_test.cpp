#include <ssoa/service/response.h>
#include <ssoa/service/serviceargument.h>

#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

using namespace ssoa;
using std::string;
using std::vector;

typedef unsigned char byte;

class MyResponse: public Response
{
public:
    MyResponse(ServiceSignature signature) :
        Response(std::move(signature))
    {
    }
    void getConstBuffers() {
        Response::getConstBuffers();
    }
};

BOOST_AUTO_TEST_SUITE(response)

    BOOST_AUTO_TEST_CASE( serviceargument_int_double_constructors_test )
    {
        MyResponse m("RotateImage(out int, out double)");
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        BOOST_CHECK_THROW(m.pushArgument(new ServiceDoubleArgument(1.0)), std::logic_error);
        BOOST_CHECK_NO_THROW(m.pushArgument(new ServiceIntArgument(1)));
        BOOST_CHECK_THROW(m.popArgument<ServiceIntArgument>(), std::logic_error);
        BOOST_CHECK_NO_THROW(m.pushArgument(new ServiceDoubleArgument(1.0)));
        BOOST_CHECK_THROW(m.pushArgument(new ServiceDoubleArgument(1.0)), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());

        BOOST_CHECK_THROW(m.popArgument<ServiceDoubleArgument>(), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());
        ServiceIntArgument *i = NULL;
        BOOST_CHECK_NO_THROW(i = m.popArgument<ServiceIntArgument>());
        BOOST_CHECK_EQUAL(i->getValue(), 1);
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        ServiceDoubleArgument *d = NULL;
        BOOST_CHECK_NO_THROW(d = m.popArgument<ServiceDoubleArgument>());
        BOOST_CHECK_EQUAL(d->getValue(), 1.0);
        BOOST_CHECK_THROW(m.popArgument<ServiceDoubleArgument>(), std::logic_error);
        BOOST_CHECK_THROW(m.popArgument<ServiceIntArgument>(), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE( serviceargument_string_constructor_test )
    {
        MyResponse m("RotateImage(out string)");
        string text("hello");
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        BOOST_CHECK_THROW(m.pushArgument(new ServiceDoubleArgument(0.0)), std::logic_error);
        BOOST_CHECK_NO_THROW(m.pushArgument(new ServiceStringArgument(text)));
        BOOST_CHECK_THROW(m.pushArgument(new ServiceStringArgument("invalid!")), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());

        BOOST_CHECK_THROW(m.popArgument<ServiceDoubleArgument>(), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());
        ServiceStringArgument *s = NULL;
        BOOST_CHECK_NO_THROW(s = m.popArgument<ServiceStringArgument>());
        BOOST_CHECK_EQUAL(s->getValue(), text);
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(s->getData()), text.size());
        BOOST_CHECK_EQUAL(strcmp(boost::asio::buffer_cast<char*>(s->getData()), text.c_str()), 0);
        BOOST_CHECK_THROW(m.popArgument<ServiceStringArgument>(), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE( serviceargument_buffer_constructor_test )
    {
        MyResponse m("RotateImage(out buffer)");
        vector<byte> buffer { 1, 2, 3 };
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        BOOST_CHECK_THROW(m.pushArgument(new ServiceDoubleArgument(0.0)), std::logic_error);
        BOOST_CHECK_NO_THROW(m.pushArgument(new ServiceBufferArgument(buffer)));
        BOOST_CHECK_THROW(m.pushArgument(new ServiceBufferArgument(buffer)), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());

        BOOST_CHECK_THROW(m.popArgument<ServiceDoubleArgument>(), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());
        ServiceBufferArgument *ba = NULL;
        BOOST_CHECK_NO_THROW(ba = m.popArgument<ServiceBufferArgument>());
        auto& b = ba->getValue();
        BOOST_CHECK_EQUAL_COLLECTIONS(buffer.begin(), buffer.end(), b.begin(), b.end());
        BOOST_CHECK_THROW(m.popArgument<ServiceBufferArgument>(), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE( serviceargument_empty_buffer_constructor_test )
    {
        MyResponse m("RotateImage(out buffer)");
        vector<byte> buffer;
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        BOOST_CHECK_THROW(m.pushArgument(new ServiceDoubleArgument(0.0)), std::logic_error);
        BOOST_CHECK_NO_THROW(m.pushArgument(new ServiceBufferArgument(buffer)));
        BOOST_CHECK_THROW(m.pushArgument(new ServiceBufferArgument(buffer)), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());

        BOOST_CHECK_THROW(m.popArgument<ServiceDoubleArgument>(), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());
        ServiceBufferArgument *ba = NULL;
        BOOST_CHECK_NO_THROW(ba = m.popArgument<ServiceBufferArgument>());
        auto& b = ba->getValue();
        BOOST_CHECK_EQUAL_COLLECTIONS(buffer.begin(), buffer.end(), b.begin(), b.end());
        BOOST_CHECK_THROW(m.popArgument<ServiceBufferArgument>(), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE( serviceargument_int_double_prepare_test )
    {
        MyResponse m("RotateImage(in double, out int, out double)");
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        ServiceDoubleArgument *d = dynamic_cast<ServiceDoubleArgument*>(ServiceDoubleArgument::prepare(sizeof(double)));
        *d = 2;
        ServiceIntArgument *i = dynamic_cast<ServiceIntArgument*>(ServiceIntArgument::prepare(sizeof(int32_t)));
        *i = 4;
        BOOST_CHECK_THROW(m.pushArgument(d), std::logic_error);
        BOOST_CHECK_NO_THROW(m.pushArgument(i));
        BOOST_CHECK_NO_THROW(m.pushArgument(d));
        BOOST_CHECK_THROW(m.pushArgument(d), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());

        BOOST_CHECK_THROW(ServiceDoubleArgument *dd = m.popArgument<ServiceDoubleArgument>(), std::logic_error);
        BOOST_CHECK_NO_THROW(m.getConstBuffers());
        ServiceIntArgument *ii = NULL;
        BOOST_CHECK_NO_THROW(ii = m.popArgument<ServiceIntArgument>());
        BOOST_CHECK_EQUAL(ii, i);
        BOOST_CHECK_EQUAL(ii->getValue(), 4);
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(ii->getData()), sizeof(int32_t));
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        ServiceDoubleArgument *dd = NULL;
        BOOST_CHECK_NO_THROW(dd = m.popArgument<ServiceDoubleArgument>());
        BOOST_CHECK_EQUAL(dd, d);
        BOOST_CHECK_EQUAL(dd->getValue(), 2);
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(dd->getData()), sizeof(double));
        BOOST_CHECK_THROW(ServiceDoubleArgument *d = m.popArgument<ServiceDoubleArgument>(), std::logic_error);
        BOOST_CHECK_THROW(ServiceIntArgument *i = m.popArgument<ServiceIntArgument>(), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE( serviceargument_string_prepare_strcpy_test )
    {
        MyResponse m("RotateImage(in double, out string)");
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        string text("hello");
        ServiceStringArgument *s = dynamic_cast<ServiceStringArgument*>(ServiceStringArgument::prepare(text.size()));
        BOOST_CHECK_EQUAL(s->getValue().c_str(), string("").c_str()); // c_str(): strings are actually different (in size)
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(s->getData()), text.size());
        // strcpy copies the terminating '\0': no problem with string size
        strcpy(boost::asio::buffer_cast<char*>(s->getData()), text.c_str());
        BOOST_CHECK_EQUAL(s->getValue(), text);
        BOOST_CHECK_NO_THROW(m.pushArgument(s));
        BOOST_CHECK_NO_THROW(m.getConstBuffers());

        ServiceStringArgument *ss = NULL;
        BOOST_CHECK_NO_THROW(ss = m.popArgument<ServiceStringArgument>());
        BOOST_CHECK_EQUAL(ss, s);
        BOOST_CHECK_EQUAL(ss->getValue(), text);
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(ss->getData()), text.size());
        BOOST_CHECK_EQUAL(strcmp(boost::asio::buffer_cast<char*>(ss->getData()), text.c_str()), 0);
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE( serviceargument_string_prepare_memcpy_test )
    {
        MyResponse m("RotateImage(in double, out string)");
        string text("hello");
        ServiceStringArgument *s = dynamic_cast<ServiceStringArgument*>(ServiceStringArgument::prepare(text.size()));
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(s->getData()), text.size());
        // memcpy does not copy '\0': it is OK, the ending '\0' was set by string.resize() in prepare().
        memcpy(boost::asio::buffer_cast<char*>(s->getData()), text.c_str(), text.size());
        BOOST_CHECK_EQUAL(s->getValue(), text);
        BOOST_CHECK_NO_THROW(m.pushArgument(s));
        BOOST_CHECK_NO_THROW(m.getConstBuffers());
        BOOST_CHECK_NO_THROW(m.popArgument<ServiceStringArgument>());
    }

    BOOST_AUTO_TEST_CASE( serviceargument_empty_string_prepare_test )
    {
        MyResponse m("RotateImage(in double, out string)");
        string text("hello");
        ServiceStringArgument *s = dynamic_cast<ServiceStringArgument*>(ServiceStringArgument::prepare(0));
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(s->getData()), 0);
        memcpy(boost::asio::buffer_cast<char*>(s->getData()), "", 0);
        BOOST_CHECK_EQUAL(s->getValue(), string(""));
        BOOST_CHECK_NO_THROW(m.pushArgument(s));
        BOOST_CHECK_NO_THROW(m.getConstBuffers());
    }

    BOOST_AUTO_TEST_CASE( serviceargument_buffer_prepare_test )
    {
        MyResponse m("RotateImage(in double, out buffer)");
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
        vector<byte> buf { 1, 2, 3, 4 };
        ServiceBufferArgument *b = dynamic_cast<ServiceBufferArgument*>(ServiceBufferArgument::prepare(buf.size()));
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(b->getData()), buf.size());
        memcpy(boost::asio::buffer_cast<char*>(b->getData()), buf.data(), buf.size());
        BOOST_CHECK_EQUAL_COLLECTIONS(buf.begin(), buf.end(), b->getValue().begin(), b->getValue().end());
        BOOST_CHECK_NO_THROW(m.pushArgument(b));
        BOOST_CHECK_NO_THROW(m.getConstBuffers());

        ServiceBufferArgument *bb = NULL;
        BOOST_CHECK_NO_THROW(bb = m.popArgument<ServiceBufferArgument>());
        BOOST_CHECK_EQUAL(bb, b);
        BOOST_CHECK_THROW(m.getConstBuffers(), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE( serviceargument_empty_buffer_prepare_test )
    {
        MyResponse m("RotateImage(in double, out buffer)");
        vector<byte> empty;
        ServiceBufferArgument *b = dynamic_cast<ServiceBufferArgument*>(ServiceBufferArgument::prepare(0));
        BOOST_CHECK_EQUAL(boost::asio::buffer_size(b->getData()), 0);
        memcpy(boost::asio::buffer_cast<char*>(b->getData()), empty.data(), 0);
        BOOST_CHECK_EQUAL_COLLECTIONS(empty.begin(), empty.end(), b->getValue().begin(), b->getValue().end());
        BOOST_CHECK_NO_THROW(m.pushArgument(b));
        BOOST_CHECK_NO_THROW(m.getConstBuffers());
    }

    BOOST_AUTO_TEST_SUITE_END()
