#include <boost/test/unit_test.hpp>

#include <identity.hpp>

#include <config.hpp>
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

  std::string bitcoin_content =
R"(# Web of trust configuration file

# Possible algoritms for public key and signature accepted
#"nostr" will be here
algos = [ "bitcoin" ]

signer = "electrum"

verifier = "electrum"
)";

  std::string both_content_1 =
R"(# Web of trust configuration file

# Possible algoritms for public key and signature accepted
#"nostr" will be here
algos = [ "bitcoin", "dummy" ]

signer = "electrum"

verifier = "electrum"
)";

  std::string both_content_2 =
R"(# Web of trust configuration file

# Possible algoritms for public key and signature accepted
#"nostr" will be here
algos = [ "dummy", "bitcoin" ]

signer = "electrum"

verifier = "electrum"
)";

} // namespace

using namespace wot;

BOOST_AUTO_TEST_SUITE(Identity_suite);

BOOST_AUTO_TEST_CASE(exec1) {

  BOOST_TEST_MESSAGE("Writing a config test file");
  DiskDb::generic_write_file("/tmp/test_file_config.toml",bitcoin_content);
  BOOST_CHECK(Config::get().load("/tmp/test_file_config.toml"));

  Identity i("aa");
  BOOST_CHECK( i.is_well_formed() == false );

  Identity j("bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq");
  BOOST_CHECK( j.is_well_formed() == true );

  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_CASE(exec2) {
  BOOST_TEST_MESSAGE("Writing a config test file");
  DiskDb::generic_write_file("/tmp/test_file_config.toml",dummy_content);
  BOOST_CHECK(Config::get().forced_load("/tmp/test_file_config.toml"));

  Identity i("aa");
  BOOST_CHECK( i.is_well_formed() == true );

  Identity j("bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq");
  BOOST_CHECK( j.is_well_formed() == true );

  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_CASE(exec3) {
  BOOST_TEST_MESSAGE("Writing a config test file");
  DiskDb::generic_write_file("/tmp/test_file_config.toml",both_content_1);
  BOOST_CHECK(Config::get().forced_load("/tmp/test_file_config.toml"));

  Identity i("aa");
  BOOST_CHECK( i.is_well_formed() == true );

  Identity j("bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq");
  BOOST_CHECK( j.is_well_formed() == true );

  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_CASE(exec4) {
  BOOST_TEST_MESSAGE("Writing a config test file");
  DiskDb::generic_write_file("/tmp/test_file_config.toml",both_content_2);
  BOOST_CHECK(Config::get().forced_load("/tmp/test_file_config.toml"));

  Identity i("aa");
  BOOST_CHECK( i.is_well_formed() == true );

  Identity j("bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq");
  BOOST_CHECK( j.is_well_formed() == true );

  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_SUITE_END()
