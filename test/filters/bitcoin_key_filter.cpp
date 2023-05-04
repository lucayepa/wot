#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <filters/bitcoin_key_filter.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <config.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(BitcoinKeyFilterTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_CHECK(Config::get().load());

  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",s));
  Node n(s);

  //Need to verify the node to fill up n
  BOOST_CHECK(n.verify_node(true,true));

  BOOST_CHECK(BitcoinKeyFilter().check(n, ""));

  //Should try with a node that does not pass the check (TODO)
}

BOOST_AUTO_TEST_SUITE_END()
