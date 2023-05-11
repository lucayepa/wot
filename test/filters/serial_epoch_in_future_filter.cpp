#include <boost/test/unit_test.hpp>

#include <filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <config.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(SerialEpochInFutureFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  Filter* f = Config::get().get_filters()["SerialEpochInFutureFilter"];

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  BOOST_CHECK(!f->check(n));
}

BOOST_AUTO_TEST_SUITE_END()
