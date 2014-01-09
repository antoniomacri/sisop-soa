#define BOOST_TEST_MODULE servicesignature_test
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <ssoa/service/servicesignature.h>

#include <boost/test/unit_test.hpp>

using namespace ssoa;

namespace std
{
    ostream & operator <<(ostream & o, const ServiceSignature & signature)
    {
        o << (string)signature;
        return o;
    }
}

BOOST_AUTO_TEST_CASE( constructors_test )
{
    ServiceSignature siga("RotateImage(in int, in buffer, out buffer)");
    ServiceSignature sigb("  RotateImage   (   in   int  ,  in  buffer  ,  out  buffer  )  ");
    BOOST_CHECK(siga.isValid());
    BOOST_CHECK(sigb.isValid());
    BOOST_CHECK_EQUAL(siga.getName(), "RotateImage");
    BOOST_CHECK_EQUAL(sigb.getName(), "RotateImage");
    BOOST_CHECK_EQUAL(siga, sigb);

    // A service may have just one input or output parameter
    BOOST_CHECK(ServiceSignature("RotateImage(in int)").isValid());
    BOOST_CHECK(ServiceSignature("RotateImage(out int)").isValid());
    BOOST_CHECK(ServiceSignature("RotateImage(in double)").isValid());
    BOOST_CHECK(ServiceSignature("RotateImage(in string)").isValid());
    BOOST_CHECK(ServiceSignature("RotateImage(in buffer)").isValid());

    BOOST_CHECK(!ServiceSignature("RotateImage()").isValid());
    BOOST_CHECK(!ServiceSignature("RotateImage(,)").isValid());
    BOOST_CHECK(!ServiceSignature("Rotate Image(in int, in buffer)").isValid());
    BOOST_CHECK(!ServiceSignature("RotateImage(int int, in buffer)").isValid());
    BOOST_CHECK(!ServiceSignature("RotateImage(in int in buffer)").isValid());
    BOOST_CHECK(!ServiceSignature("RotateImage(in int, int buffer)").isValid());
}

BOOST_AUTO_TEST_CASE( params_test )
{
    ServiceSignature sigc("RotateImage(in intt, in buffer)");
    BOOST_CHECK(sigc.isValid());
    BOOST_CHECK_EQUAL(sigc.getInputParams().size(), 2);
    BOOST_CHECK_EQUAL(sigc.getInputParams()[0], "intt");
    BOOST_CHECK_EQUAL(sigc.getInputParams()[1], "buffer");
    BOOST_CHECK_EQUAL(sigc.getOutputParams().size(), 0);

    ServiceSignature sigd("RotateImage(in int , out  buffers )");
    BOOST_CHECK(sigd.isValid());
    BOOST_CHECK_EQUAL(sigd.getInputParams().size(), 1);
    BOOST_CHECK_EQUAL(sigd.getInputParams()[0], "int");
    BOOST_CHECK_EQUAL(sigd.getOutputParams().size(), 1);
    BOOST_CHECK_EQUAL(sigd.getOutputParams()[0], "buffers");
}
