#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <commands/add_command.hpp>
#include <commands/compose_command.hpp>

#include <string>
#include <node.hpp>
#include <identity.hpp>
#include <disk_db.hpp>

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

BOOST_AUTO_TEST_SUITE(AddCommandTest)

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

  vm.emplace("to",po::variable_value((std::string)"to", true));
  vm.emplace("unit",po::variable_value((std::string)"USD", true));
  vm.emplace("value",po::variable_value((int)50, true));
  vm.emplace("since",po::variable_value((int)8, true));

  vm.emplace("version",po::variable_value((std::string)"0.1", true));

  nlohmann::json j;

  ComposeCommand().fill_with(vm, j);
  ComposeCommand().complete(j);

  Node n(j);

  BOOST_CHECK( DbNodes().add(n) );

  std::string signed_object = R"({"circle":"friends","version":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7","sig":"dummy-sig-for-7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})";

  // Start from the artifact that arrives from sign test
  Node m(signed_object);
  // Verify the new node
  DiskDb().rm(m.get_signature().get_hash());
  DiskDb("sig").add(m.get_signature().get_hash(), m.get_signature().get_sig());
  BOOST_CHECK( m.verify_node() );

  BOOST_CHECK( DbNodes().add(m) );

  // We should check the output of the following, that should be == to signed_object
  DiskDb((std::string)"").print("d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08");

  vm_t bitcoin_vm;
  bitcoin_vm.emplace("bitcoin-key-filter",po::variable_value((std::string)"1", true));
  BOOST_CHECK( ! DbNodes().add(m, bitcoin_vm) );

  std::string not_signed_object = R"({"circle":"friends2","version":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"1234","sig":"dummy-sig-for-1234"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})";
  Node m2(not_signed_object);

  // No verify, because hash and signature are wrong. Directly add.
  BOOST_CHECK( !DbNodes().add(m2, bitcoin_vm) );

  vm_t empty_vm;
  Node m3(not_signed_object);
  // No verify, because hash and signature are wrong
  BOOST_CHECK( DbNodes().add(m3, empty_vm) );

  DiskDb().rm(m.get_signature().get_hash());
  DiskDb("orig").rm(m.get_signature().get_hash());
  DiskDb("orig").rm("");
  DiskDb("sig").rm(m.get_signature().get_hash());
  DiskDb().rm("1234");
  DiskDb("orig").rm("1234");

  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_SUITE_END()
