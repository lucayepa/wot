#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <config.hpp>

#include <filter.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(ToFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  Filter* f = Config::get().get_filters()["ToFilter"];

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  std::vector<std::string> v {
    "bc1qvhg867v620d7vtg4e5qnuh5py8esqhs65w0r4c",
    "12c6DSiU4Rq3P4ZxziKxzrL5LmMBrzjrJX"
  };

  BOOST_CHECK(f->check(n, v));
  v = {
    "bc1qvhg867v620d7vtg4e5qnuh5py8esqhs65w0r4c"
  };
  BOOST_CHECK(f->check(n, v));
  v = {
    "wrong-key"
  };
  BOOST_CHECK(!f->check(n, v));
  v = {
    "12c6DSiU4Rq3P4ZxziKxzrL5LmMBrzjrJX",
    "wrong-key"
  };
  BOOST_CHECK(!f->check(n, v));
}

BOOST_AUTO_TEST_SUITE_END()
