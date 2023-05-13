#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <config.hpp>

COMMAND_START(SignTomlCommand)
  COMMAND_CLI("sign-toml")
  COMMAND_SHORT_DESCRIPTION("Sign a node in TOML format")
  COMMAND_DESCRIPTION(R"(
  Get a TOML node from stdin, add a valid hash and a valid signature.

  --json-output return a json object (with original TOML signature)

  Experts only:

  --force-accept-hash do not check the hash of the object

  See also `help sign`.
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    string s, json, toml;
    if(!Config::get_input(s)) return false;
    Node n(s);
    std::string res;
    try {
      res = n.get_signed(vm.count("force-accept-hash"));
      if(vm.count("json-output")) {
          std::cerr <<
          "Json signed object. This object can be verified only if the "
          "TOML source is present. To verify a toml object, see `help verify`."
          << std::endl;
      } else {
          std::cerr << "TOML signed object. To verify a toml object, see "
          "`help verify`." << std::endl;
      }
      std::cout << res;
      return true;
    } catch (...) {
      std::cerr << "NOT ok" << std::endl;
      return false;
    }
  }
COMMAND_END()
