#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <db_nodes.hpp>

COMMAND_START(LsCommand)
  COMMAND_CLI("ls")
  COMMAND_SHORT_DESCRIPTION("List of the objects in the internal db filtered against the provided filters.")
  COMMAND_SYNOPSIS("wot [--rule-filter RULE] [--to-filter TO] [from-filter FROM] ls")
  COMMAND_DESCRIPTION(R"(
  List of the objects in the internal db filtered against the provided filters.

  BUG: The objects are first filtered, then are listed as full object.
  This means that if a single link of an object pass the filter, then all the links are shown.
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    Db_nodes().list_nodes(vm);
    return true;
  }
COMMAND_END()
