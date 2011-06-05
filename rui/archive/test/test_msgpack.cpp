#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include "msgpack_archive.hpp"

struct A {
    int a;
    float b;

    A() : a(-1), b(1.234) {}

    template <class Stream>
    void pack(msgpack::packer<Stream>& p) {
        p << a << b;
    }

    template <class Archive>
    void serialize(Archive& ar, int) {
        ar & a & b;
    }
};

struct B {
    A a;
    bool b;

    B() : b(false) {}

    template <class Stream>
    void pack(msgpack::packer<Stream>& p) {
        a.pack(p);
        p << b;
    }

    template <class Archive>
    void serialize(Archive& ar, int) {
        ar & a & b;
    }
};

BOOST_AUTO_TEST_CASE(pack) {
    std::stringstream ss1, ss2;

    {
        B b;
        test_oarchive oa(ss1);
        oa << b;
    }

    {
        B b;
        msgpack::packer<std::stringstream> p(ss2);
        b.pack(p);
    }

    std::string s1 = ss1.str();
    std::string s2 = ss2.str();

    BOOST_REQUIRE(s1.size() >= s2.size());
    BOOST_CHECK(s1.substr(s1.size() - s2.size()) == s2);
}
