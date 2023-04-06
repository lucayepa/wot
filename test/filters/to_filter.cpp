#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <filters/to_filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(ToFilterTest)

BOOST_AUTO_TEST_CASE(check) {
  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../test/node1s.toml",s));
  Node n(s);

  //Need to verify the node to fill it up
  BOOST_CHECK(n.verify_node(true,true));

  BOOST_CHECK(ToFilter().check(n, "bc1qvhg867v620d7vtg4e5qnuh5py8esqhs65w0r4c"));
}

BOOST_AUTO_TEST_SUITE_END()
