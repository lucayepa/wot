#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <config.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(OnFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  Filter<NodeBase>* f = Config::get().get_filters()["OnFilter"];

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  BOOST_CHECK(f->check(n, "https://gist.github.com/lucayepa/3bec3ba83fcc9a576aa79a7f16326be3"));
}

BOOST_AUTO_TEST_SUITE_END()
