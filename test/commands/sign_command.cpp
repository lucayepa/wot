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

  vm.emplace("implementation",po::variable_value((std::string)"0.1", true));

  vm.emplace("profile-key",po::variable_value((std::string)"dummy-key", true));

  nlohmann::json j;

  ComposeCommand().fill_with(vm, j);
  ComposeCommand().complete(j);

  Node n(j.dump());

  // This fails (TODO)
  //BOOST_CHECK( !n.get_signed(/*as_toml=*/false, /*force-accept-hash=*/false).has_value() );

  // Calling twice should give the same result
  // This fails (TODO)
  //BOOST_CHECK( !n.get_signed(/*as_toml=*/false, /*force-accept-hash=*/false).has_value() );

  // How to get a hash for a dummy signature? For a bitcoin one, we can use get_signed that
  // inject in the node the signature and the hash. For a dummy signature, this doesn't work.
  // TODO
  // add signature for dummy-key and hash d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08
  DiskDb("sig").add("d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08", "dummy-sig-for-d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08");

  BOOST_CHECK( n.get_signed(/*as_toml=*/false, /*force-accept-hash=*/false).has_value() );
  // Calling twice should give the same result
  BOOST_CHECK( n.get_signed(/*as_toml=*/false, /*force-accept-hash=*/false).has_value() );

  std::string signed_object = n.get_signed(/*as_toml=*/false, /*force-accept-hash=*/false).value();
  BOOST_CHECK(signed_object == R"({"circle":"friends","implementation":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08","sig":"dummy-sig-for-d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})");

  // This fails because is based on the first object in string (TODO)
  // BOOST_CHECK( n.verify_node(/*force-accept-hash=*/false,/*force-accept-sig=*/false) );
  // get_json is correct with the last version
  BOOST_CHECK(n.get_json() == signed_object);

  // Create a new node with the correct get_json
  Node m(n.get_json());
  // Verify the new node
  BOOST_CHECK( m.verify_node(/*force-accept-hash=*/false,/*force-accept-sig=*/false) );

  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_SUITE_END()
