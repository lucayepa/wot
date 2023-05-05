#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <config.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(RuleFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  Filter* f = Config::get().get_filters()["RuleFilter"];

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  //Need to verify the node to fill it up
  BOOST_CHECK(n.verify_node(true,true));

  BOOST_CHECK(f->check(n, "https://gist.github.com/lucayepa/3bec3ba83fcc9a576aa79a7f16326be3"));
}

BOOST_AUTO_TEST_SUITE_END()
