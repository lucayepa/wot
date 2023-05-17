#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <config.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(HashFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  Filter<NodeBase>* f = Config::get().get_filters()["HashFilter"];

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  BOOST_CHECK(f->check(n, n.get_signature().get_hash()));
}

BOOST_AUTO_TEST_SUITE_END()
