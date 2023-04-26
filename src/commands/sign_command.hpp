#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <config.hpp>

COMMAND_START(SignCommand)
  COMMAND_CLI("sign")
  COMMAND_SHORT_DESCRIPTION("Sign a node")
  COMMAND_DESCRIPTION(R"(
  Get an object from stdin, add a valid hash and a valid signature.

  --force-accept-hash do not check the hash of the object

  See also `help sign-toml`.
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    string s, json;
    if(!Config::get_input(s)) return false;
    Node n(s);
    auto result = n.get_signed(/*as_toml=*/false, vm.count("force-accept-hash"));
    if(result.has_value()) {
      std::cout << "Signed object:" << std::endl << result.value() << std::endl;
      return true;
    } else {
      std::cout << "NOT ok" << std::endl;
      return false;
    }
  }
COMMAND_END()
