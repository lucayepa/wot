#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <config.hpp>

COMMAND_START(VerifyCommand)
  COMMAND_CLI("verify")
  COMMAND_SHORT_DESCRIPTION("Verify if a node is valid")
  COMMAND_DESCRIPTION(R"(
  Get a node from stdin and verify if is valid.

  --force-accept-hash do not check the hash of the object
  --force-accept-sig do not check the signature of the object
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    string s;
    if(!Config::get_input(s)) return false;
    Node n(s);
    bool v = n.verify_node(vm.count("force-accept-hash"),vm.count("force-accept-sig"));
    if(v) {
      std::cout << "ok" << std::endl;
      return true;
    } else {
      std::cout << "NOT ok" << std::endl;
      return false;
    }
  }
COMMAND_END()
