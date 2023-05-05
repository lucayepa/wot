#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <config.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(FromFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  Filter* f = Config::get().get_filters()["FromFilter"];

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  //Need to verify the node to fill up n
  BOOST_CHECK(n.verify_node(true,true));

  BOOST_CHECK(f->check(n, "bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq"));
}

BOOST_AUTO_TEST_SUITE_END()
