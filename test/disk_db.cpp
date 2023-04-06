#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <disk_db.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(DiskDbTest)

BOOST_AUTO_TEST_CASE(generic_read_file) {
  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("/etc/passwd",s));
  BOOST_CHECK(boost::algorithm::contains(s, "root"));

  BOOST_TEST_MESSAGE("This is false because current dir is build dir");
  BOOST_CHECK( ! DiskDb::generic_read_file("test/node1s.toml",s));

  BOOST_CHECK(DiskDb::generic_read_file("../test/node1s.toml",s));

  BOOST_CHECK(boost::algorithm::contains(s, "circle"));
}

BOOST_AUTO_TEST_SUITE_END()
