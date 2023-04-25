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
    DiskDb("sig").rm(vm["param"].as<string>());
    return true;
  }

  bool args_ok(const boost::program_options::variables_map & vm) const override {
    return(vm.count("param"));
  }

COMMAND_END()
