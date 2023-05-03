#include <boost/test/unit_test.hpp>
#include <boost/program_options.hpp>

#include <nlohmann/json.hpp>

#include <commands/compose_command.hpp>

#include <string>
#include <node.hpp>
#include <disk_db.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(ComposeCommandTest)

BOOST_AUTO_TEST_CASE(check) {

  namespace po = boost::program_options;

  vm_t vm;

  vm.emplace("serial",po::variable_value((int)7, true));

  nlohmann::json n;

  ComposeCommand().fill_with(vm, n);
  BOOST_CHECK( n.dump() == R"({"serial":7,"signature":{"hash":"","sig":""}})" );

  vm.emplace("circle",po::variable_value((std::string)"friends", true));
  ComposeCommand().fill_with(vm, n);
  BOOST_CHECK( n.dump() == R"({"circle":"friends","serial":7,"signature":{"hash":"","sig":""}})" );

  vm.emplace("to",po::variable_value((std::string)"to", true));
  vm.emplace("unit",po::variable_value((std::string)"USD", true));
  vm.emplace("value",po::variable_value((int)50, true));
  vm.emplace("since",po::variable_value((int)8, true));
  ComposeCommand().fill_with(vm, n);
  BOOST_CHECK( n.dump() == R"({"circle":"friends","serial":7,"signature":{"hash":"","sig":""},"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})" );
  ComposeCommand().complete(n);
  BOOST_CHECK( n.dump() == R"({"circle":"friends","implementation":"","profile":{"about":"","aka":"","dob":"","facebook":"","key":"","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"","sig":""},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})" );

  BOOST_TEST_MESSAGE("Twice with the same vm will add a new link");
  vm.emplace("implementation",po::variable_value((std::string)"0.1", true));
  ComposeCommand().fill_with(vm, n);
  BOOST_CHECK( n.dump() == R"({"circle":"friends","implementation":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"","sig":""},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50},{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})" );

}

BOOST_AUTO_TEST_SUITE_END()
