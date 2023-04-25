#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <db_nodes.hpp>

COMMAND_START(LsRulesCommand)
  COMMAND_CLI("ls-rules")
  COMMAND_SHORT_DESCRIPTION("List all the rules found in the links of the objects of the internal db")
  COMMAND_DESCRIPTION(R"(
  List all the rules found in the links of the objects of the internal db.
  The number next to every rule is how many times it is present in the links.
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    Db_nodes().list_rules();
    return true;
  }
COMMAND_END()
