#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <commands/verify_command.hpp>
#include <commands/compose_command.hpp>

#include <string>
#include <node.hpp>
#include <identity.hpp>
#include <disk_db.hpp>
#include <db_nodes.hpp>

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

BOOST_AUTO_TEST_SUITE(VerifyCommandTest)

BOOST_AUTO_TEST_CASE(check) {
  BOOST_TEST_MESSAGE("Writing a config test file");
  DiskDb::generic_write_file("/tmp/test_file_config.toml",dummy_content);
  BOOST_CHECK(Config::get().load("/tmp/test_file_config.toml"));

  Identity i("aa");
  BOOST_CHECK( i.is_well_formed() == true );

  namespace po = boost::program_options;

  vm_t vm;

  vm.emplace("serial",po::variable_value((int)7, true));

  vm.emplace("circle",po::variable_value((std::string)"friends", true));

  nlohmann::json j;

  ComposeCommand().fill_with(vm, j);
  ComposeCommand().complete(j);

  std::string j_should_be = R"({"circle":"friends","profile":{"about":"","aka":"","dob":"","facebook":"","key":"","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"","sig":""},"sources":[],"trust":[],"version":""})";
  BOOST_CHECK(j.dump() == j_should_be);

  Node n(j);

  // Normal verification should fail because of hash that is missing
  BOOST_CHECK( ! n.verify_node() );

  vm.emplace("force-accept-hash",po::variable_value((std::string)"1", true));

  // If we skip the hash check, it still is false, because the dummy algo wants the
  // signature in the cache of signatures
  BOOST_CHECK( !n.verify_node(vm) );

  std::string signed_object = R"({"circle":"friends","version":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7","sig":"dummy-sig-for-7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})";

  // Start again, with an artifact made with sign and having the right hash,
  // with the right signature because of dummy algo
  Node m(signed_object);

  // Now hash is good
  // Verify not working because the signature is missing from the signature cache.
  DiskDb().rm(m.get_signature().get_hash());
  DiskDb("sig").rm(m.get_signature().get_hash());
  BOOST_CHECK( !m.verify_node() );

  DiskDb("sig").add(m.get_signature().get_hash(), m.get_signature().get_sig());
  BOOST_CHECK( m.verify_node() );
  DiskDb("sig").rm(m.get_signature().get_hash());

  // Signature accepted because of the force option
  vm_t vm2;
  vm2.emplace("force-accept-sig",po::variable_value((std::string)"1", true));
  BOOST_CHECK( m.verify_node(vm2) );

  // Keep the signature ok
  DiskDb("sig").add(m.get_signature().get_hash(), m.get_signature().get_sig());
  DiskDb().rm(m.get_signature().get_hash());
  // Add the node to the db (it will verify)
  BOOST_CHECK( DbNodes().add(m) );
  // Now node is not ok, because it is a replication of one already in db
  BOOST_CHECK( !m.verify_node() );

  // Force to accept it
  vm2.emplace("force-no-db-check",po::variable_value((std::string)"1", true));
  BOOST_CHECK( m.verify_node(vm2) );

  std::string old_object = R"({"circle":"friends","version":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":4,"signature":{"hash":"1d959bb4ec960444583b0f20b00293c6588bf92d160a83a6e6ce5a7ebb0db9ea","sig":"dummy-sig-for-1d959bb4ec960444583b0f20b00293c6588bf92d160a83a6e6ce5a7ebb0db9ea"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})";
  Node old(old_object);
  DiskDb().rm(old.get_signature().get_hash());
  DiskDb("sig").add(old.get_signature().get_hash(), old.get_signature().get_sig());

  // Refused
  BOOST_CHECK( !old.verify_node() );
  vm_t vm3;
  vm3.emplace("force-accept-hash",po::variable_value((std::string)"1", true));
  vm3.emplace("force-accept-sig",po::variable_value((std::string)"1", true));
  // Refused
  BOOST_CHECK( !old.verify_node(vm3) );
  // Force to accept it
  vm3.emplace("force-no-db-check",po::variable_value((std::string)"1", true));
  BOOST_CHECK( old.verify_node(vm3) );

  DiskDb().rm(m.get_signature().get_hash());
  DiskDb("orig").rm(m.get_signature().get_hash());
  DiskDb("sig").rm(m.get_signature().get_hash());
  DiskDb("sig").rm(old.get_signature().get_hash());
  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_SUITE_END()
