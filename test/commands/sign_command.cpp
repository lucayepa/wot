#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <commands/compose_command.hpp>
#include <commands/sign_command.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>
#include <identity.hpp>

using namespace wot;

std::string dummy_content =
R"(# Web of trust configuration file

# Possible algoritms for public key and signature accepted
#"nostr" will be here
algos = [ "dummy" ]

signer = "electrum"

verifier = "electrum"
)";

BOOST_AUTO_TEST_SUITE(SignCommandTest)

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

  vm.emplace("profile-key",po::variable_value((std::string)"dummy-key", true));

  nlohmann::json j;

  ComposeCommand().fill_with(vm, j);
  ComposeCommand().complete(j);

  Node n(j.dump());

  std::string hash = "7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7";
  DiskDb("sig").rm(n.get_signature().get_hash());

  // Dummy algo throws without a signature in the cache
  BOOST_CHECK_THROW(n.get_signed(/*force-accept-hash=*/false),std::runtime_error);

  DiskDb("sig").add(n.get_signature().get_hash(), "dummy-sig-for-" +  hash);

  std::string s = n.get_signed(/*force-accept-hash=*/false);
  BOOST_CHECK( s == R"({"circle":"friends","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7","sig":"dummy-sig-for-7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}],"version":"0.1"})" );

  s = n.get_signed(/*force-accept-hash=*/false);
  BOOST_CHECK( s == R"({"circle":"friends","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7","sig":"dummy-sig-for-7db44102834343c24ef2daacc753ae0c895a9c9aa4290e4f1b6d594183c292f7"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}],"version":"0.1"})" );

  BOOST_CHECK( n.verify_node() );
  BOOST_CHECK(n.get_json() == s);

  // Create a new node with the correct get_json
  Node m(n.get_json());
  // Verify the new node
  BOOST_CHECK( m.verify_node() );

  DiskDb("sig").rm(n.get_signature().get_hash());
  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_SUITE_END()
