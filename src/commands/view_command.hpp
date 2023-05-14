#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <db_nodes.hpp>

COMMAND_START(ViewCommand)
  COMMAND_CLI("view")
  COMMAND_SHORT_DESCRIPTION("View a node from internal db")
  COMMAND_SYNOPSIS("wot view [--json-output] <HASH>")
  COMMAND_DESCRIPTION(R"(
  View the content of the node having hash HASH, from the internal db.

  If the option "--json-output" is used, the full node in JSON format will be
  printed to stdout.
  Otherwise, just a short summary will be printed to stdout
)")

  bool act(const vm_t & vm) const override {
    std::string hash = vm["param"].as<std::string>();
    auto ndb = DbNodes();
    if(!ndb.contains(hash)) {
      std::cerr << "Hash not present in internal database" << std::endl;
      return false;
    }
    if(vm.count("json-output")) {
      ndb.print(hash);
    } else {
      NodeBase n;
      ndb.get(hash,n);
      std::cout << n;
    }
    return true;
  }

  std::pair<bool, std::string> args_ok(const vm_t & vm) const override {
    if(vm.count("param")) return Command::args_ok(vm);
    return {false, "Usage: wot view <HASH>"};
  }

COMMAND_END()
