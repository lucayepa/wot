#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <disk_db.hpp>

COMMAND_START(AddSigCommand)
  COMMAND_CLI("add-sig")
  COMMAND_SHORT_DESCRIPTION("Add a trusted signature to the internal db")
  COMMAND_SYNOPSIS("wot --signature <SIGNATURE> add-sig <HASH>")
  COMMAND_DESCRIPTION(R"(
  Add a trusted signature to the internal db. When a node having hash HASH is found,
  it is considered valid if the signature of the node is the same as SIGNATURE.
  This skips the verification step. It is useful, for example, on systems without an
  installed Electrum executable.

  See also `help ls-sig`, `help rm-sig`.
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    DiskDb("sig").add(vm["param"].as<string>(), vm["signature"].as<string>());
    return true;
  }

  bool args_ok(const boost::program_options::variables_map & vm) const override {
    return(vm.count("signature") && vm.count("param"));
  }
COMMAND_END()
