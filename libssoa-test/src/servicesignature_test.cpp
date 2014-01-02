#define BOOST_TEST_MODULE servicesignature_test
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <service/servicesignature.h>

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
    BOOST_CHECK(!ServiceSignature("RotateImage(in intt, in buffer)").isValid());
    BOOST_CHECK(!ServiceSignature("RotateImage(in int in buffer)").isValid());
    BOOST_CHECK(!ServiceSignature("RotateImage(in int, int buffer)").isValid());
    BOOST_CHECK(!ServiceSignature("RotateImage(in int, in buffers)").isValid());

    ServiceSignature::Type none[] = { ServiceSignature::TYPE_INVALID };
    ServiceSignature::Type a_string[] = { ServiceSignature::TYPE_STRING, ServiceSignature::TYPE_INVALID };
    ServiceSignature::Type a_buffer[] = { ServiceSignature::TYPE_BUFFER, ServiceSignature::TYPE_INVALID };
    BOOST_CHECK(!ServiceSignature("RotateImage", none, none).isValid());
    BOOST_CHECK(ServiceSignature("RotateImage", a_buffer, none) == ServiceSignature("RotateImage(in buffer)"));
    BOOST_CHECK(ServiceSignature("RotateImage", none, a_string) == ServiceSignature("RotateImage(out string)"));
    BOOST_CHECK(ServiceSignature("RotateImage", a_string, a_buffer)
        == ServiceSignature("RotateImage(in string, out buffer)"));
}
