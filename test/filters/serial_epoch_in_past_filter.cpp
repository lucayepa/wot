#include <boost/test/unit_test.hpp>

#include <filters/serial_epoch_in_past_filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <config.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(SerialEpochInPastFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  //Need to verify the node to fill up n
  BOOST_CHECK(n.verify_node(true,true));

  BOOST_CHECK(SerialEpochInPastFilter().check(n, ""));
}

BOOST_AUTO_TEST_SUITE_END()
