#include <boost/test/unit_test.hpp>

#include <identity.hpp>

#include <config.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(Identity_suite)

BOOST_AUTO_TEST_CASE(exec) {

  Config::get().load();

  Identity i("aa");

  BOOST_CHECK( i.is_well_formed() == false );

  Identity j("bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq");

  BOOST_CHECK( j.is_well_formed() == true );
}

BOOST_AUTO_TEST_SUITE_END()
