#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <db_nodes.hpp>
#include <config.hpp>
#include <graph.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(OnFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  Filter<Link>* f = Config::get().get_filters<Link>()["OnFilter"];

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  DbNodes().add(n);

  auto i = GraphView().get(n.get_profile().get_key());

  std::cerr << i;

  std::vector<Link> sl = i.get_trust();

  auto l1 = sl[0];
  auto l2 = sl[1];

  BOOST_CHECK(f->check(l1,"http://other.community.rules.com"));
  BOOST_CHECK(!f->check(l2,"http://other.community.rules.com"));
}

BOOST_AUTO_TEST_SUITE_END()
