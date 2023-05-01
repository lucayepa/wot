#include <boost/test/unit_test.hpp>

#include <config.hpp>
#include <disk_db.hpp>
#include <electrum.hpp>
#include <boost/algorithm/string.hpp>

using namespace wot;

const std::string test_config_file = default_content;

struct ConfigTest {
  ConfigTest() {
    BOOST_TEST_MESSAGE("Writing a config test file");
    DiskDb::generic_write_file("/tmp/test_file_config.toml",test_config_file);
  }
  ~ConfigTest() {
    BOOST_TEST_MESSAGE("Removing the config test file");
    DiskDb::generic_remove_file("/tmp/test_file_config.toml");
  }

  Config & c = Config::get();
};

// What happens if Config is not initialized with `load`?
BOOST_AUTO_TEST_SUITE(Config_suite_without_init);

BOOST_AUTO_TEST_CASE(test_config_w0) {
  BOOST_TEST_MESSAGE("In not loaded, load the default file, so signer is never NULL");
  BOOST_CHECK(Config::get().get_signer() != nullptr);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(Config_suite, ConfigTest);

//Config & c = Config::get();

BOOST_AUTO_TEST_CASE(test_config_1) {
  BOOST_TEST_MESSAGE("Get content from the test file");
  BOOST_CHECK(c.load("/tmp/test_file_config.toml"));
}

BOOST_AUTO_TEST_CASE(test_config_2) {
  BOOST_CHECK(Config::get().get_signer() != nullptr);
}

/*
BOOST_AUTO_TEST_CASE(test_config_3) {
  BOOST_TEST_MESSAGE(absolute_default_config_dir());
  BOOST_CHECK(boost::algorithm::contains(absolute_default_config_dir(),"home"));
}
*/

BOOST_AUTO_TEST_CASE(test_config_5) {
  std::string command = "some_command_name";
  c.set_command(command);
  BOOST_CHECK_EQUAL(c.get_command(), command);
}

// Next call to get doesn't do anything, but gives true
BOOST_AUTO_TEST_CASE(test_config_6) {
  BOOST_TEST_MESSAGE("Calling twice get does nothing and returns true");
  BOOST_CHECK(c.load());
}

// This destroys the config file. For the moment it is disabled.
BOOST_AUTO_TEST_CASE(test_config_7, * boost::unit_test::disabled()) {
  BOOST_TEST_MESSAGE("Beware: removing default config file from user home dir");
  DiskDb::generic_remove_file(Config::default_config_file);
  // TODO: modify HOME env to /tmp and enable the test
  // Config is a singleton, so loading a different one has no sense
  // because the fixture already loads one.
  Config & d = Config::get();
  d.load();
  BOOST_TEST_MESSAGE("Get will create a new default config file");
  BOOST_CHECK(d.load());
  BOOST_CHECK(DiskDb::generic_file_exists(Config::default_config_file));
}

BOOST_AUTO_TEST_CASE(test_config_8) {
  std::string cli = c.get().get_signer()->sign_cli("","","");
  BOOST_CHECK(boost::algorithm::contains(cli, "electrum"));
}

BOOST_AUTO_TEST_SUITE_END()
