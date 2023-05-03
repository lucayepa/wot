#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <disk_db.hpp>

COMMAND_START(ViewCommand)
  COMMAND_CLI("view")
  COMMAND_SHORT_DESCRIPTION("View a node from internal db")
  COMMAND_SYNOPSIS("wot view <HASH>")
  COMMAND_DESCRIPTION(R"(
  View the content of the node having hash HASH, from the internal db.
)")

  bool act(const vm_t & vm) const override {
    DiskDb((std::string)"").print(vm["param"].as<std::string>());
    return true;
  }

  std::pair<bool, std::string> args_ok(const vm_t & vm) const override {
    if(vm.count("param")) return Command::args_ok(vm);
    return {false, "Usage: wot view <HASH>"};
  }

COMMAND_END()
