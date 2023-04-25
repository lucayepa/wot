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

  bool act(const boost::program_options::variables_map & vm) const override {
    DiskDb((std::string)"").print(vm["param"].as<string>());
    return true;
  }

  bool args_ok(const boost::program_options::variables_map & vm) const override {
    return(vm.count("param"));
  }

COMMAND_END()
