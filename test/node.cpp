#include <boost/test/unit_test.hpp>

#include <node.hpp>
#include <disk_db.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(Node_suite)

BOOST_AUTO_TEST_CASE(exec) {
  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("node1s.toml",s));

  Node n(s);
  // Verify fill the data in the node
  BOOST_CHECK(n.verify_node(true,true));

  BOOST_CHECK(n.get_circle()!="");

  Node m(n);
  // No need to fill up when constructed based on another node
  BOOST_CHECK(m.get_circle()!="");

  std::string circle_original(m.get_circle());
  m.set_circle("a");
  BOOST_CHECK(m.get_circle()=="a");

  // Attention: verify fill it up again with original code
  BOOST_CHECK(m.verify_node(true,true));
  BOOST_CHECK(m.get_circle()==circle_original);

}

BOOST_AUTO_TEST_SUITE_END()
