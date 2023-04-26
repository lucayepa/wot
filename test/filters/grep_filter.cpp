#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <filters/grep_filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(GrepFilterTest)

BOOST_AUTO_TEST_CASE(check) {
  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  //Need to verify the node to fill it up
  BOOST_CHECK(n.verify_node(true,true));

  BOOST_CHECK(GrepFilter().check(n, n.get_signature().get_hash()));
}

BOOST_AUTO_TEST_SUITE_END()
