#include <boost/test/unit_test.hpp>

#include <graph.hpp>
#include <config.hpp>
#include <disk_db.hpp>
#include <db_nodes.hpp>
#include <node.hpp>

namespace {
std::string dummy_content =
R"(# Web of trust configuration file

# Possible algoritms for public key and signature accepted
#"nostr" will be here
algos = [ "dummy" ]

signer = "electrum"

verifier = "electrum"
)";

} // namespace

using namespace wot;

BOOST_AUTO_TEST_SUITE(Graph_suite)

BOOST_AUTO_TEST_CASE(exec) {

  BOOST_TEST_MESSAGE("Writing a config test file");
  DiskDb::generic_write_file("/tmp/test_file_config.toml",dummy_content);
  BOOST_CHECK(Config::get().load("/tmp/test_file_config.toml"));

  auto dbn = DbNodes();

  std::string s1 = R"({"circle":"friends","version":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"a","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"1234","sig":"dummy-sig-for-1234"},"sources":[],"trust":[{"on":[],"since":8,"to":"friend","unit":"USD","value":50}]})";
  Node n1(s1);
  BOOST_CHECK(n1.get_circle()=="friends");
  BOOST_CHECK(dbn.add(n1));

  // print the graph
  std::stringstream ss1;
  ss1 << GraphView();
  std::cerr << ss1.str();

  auto i = Identity(n1);
  Identity j = GraphView().get(i.get());
  std::vector<Link> trust = j.get_trust();
  BOOST_CHECK(trust[0].get_to() == "friend");
  BOOST_CHECK(trust[0].get_value() == 50);

  std::string s2 = R"({"circle":"work","version":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"b","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"12345","sig":"dummy-sig-for-12345"},"sources":[],"trust":[{"on":[],"since":8,"to":"coworker","unit":"USD","value":50}]})";
  Node n2(s2);
  BOOST_CHECK(n2.get_circle()=="work");
  BOOST_CHECK(dbn.add(n2));
  std::stringstream ss2;
  ss2 << GraphView();
  std::cerr << ss2.str();

  i = Identity(n2);
  std::vector<Link> trust2 = GraphView().get(i.get()).get_trust();
  BOOST_CHECK(trust2[0].get_to() == "friend");
  BOOST_CHECK(trust2[0].get_value() == 50);
  BOOST_CHECK(trust2[1].get_to() == "coworker");
  BOOST_CHECK(trust2[1].get_value() == 50);

  // Same circle, new serial, different value on the trust link
  std::string s3 = R"({"circle":"work","version":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":8,"signature":{"hash":"123456","sig":"dummy-sig-for-123456"},"sources":[],"trust":[{"on":[],"since":8,"to":"coworker","unit":"USD","value":10}]})";
  Node n3(s3);
  BOOST_CHECK(n3.get_circle()=="work");
  BOOST_CHECK(dbn.add(n3));
  std::stringstream ss3;
  ss3 << GraphView();
  std::cerr << ss3.str();

  i = Identity(n3);
  Identity k = GraphView().get(i.get());
  std::vector<Link> trust3 = k.get_trust();
  BOOST_CHECK(trust3[0].get_to() == "friend");
  BOOST_CHECK(trust3[0].get_value() == 50);
  BOOST_CHECK(trust3[1].get_to() == "coworker");
  BOOST_CHECK(trust3[1].get_value() == 10);

  BOOST_CHECK(dbn.rm(n1));
  BOOST_CHECK(dbn.rm(n2));
  BOOST_CHECK(dbn.rm(n3));
  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_SUITE_END()
