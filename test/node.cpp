#include <boost/test/unit_test.hpp>

#include <node.hpp>
#include <config.hpp>
#include <disk_db.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(Node_suite)

BOOST_AUTO_TEST_CASE(exec) {

  BOOST_CHECK(Config::get().load());

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("node1s.toml",s));

  Node n(s);
  BOOST_CHECK(n.get_in()==s);

  BOOST_CHECK(n.verify_node(true,true));

  BOOST_CHECK(n.get_circle()!="");

  Node m(n);
  BOOST_CHECK(m.get_circle()!="");

  std::string circle_original(m.get_circle());
  m.set_circle("a");
  BOOST_CHECK(m.get_circle()=="a");

  BOOST_CHECK(m.verify_node(true,true));
  BOOST_CHECK(!(m.get_circle()==circle_original));
}

BOOST_AUTO_TEST_SUITE_END()
