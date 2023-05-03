#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <disk_db.hpp>

COMMAND_START(RmSigCommand)
  COMMAND_CLI("rm-sig")
  COMMAND_SHORT_DESCRIPTION("Remove a trusted signature from the internal db")
  COMMAND_SYNOPSIS("wot rm-sig <HASH>")
  COMMAND_DESCRIPTION(R"(
  Remove a trusted signature from the internal db.

  See also `help add-sig`, `help ls-sig`.
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    DiskDb("sig").rm(vm["param"].as<std::string>());
    return true;
  }

  std::pair<bool, std::string> args_ok(const vm_t & vm) const override {
    if(vm.count("param")) return Command::args_ok(vm);
    return {false, "Usage: wot rm-sig <HASH>"};
  }

COMMAND_END()
