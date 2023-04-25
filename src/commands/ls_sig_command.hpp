#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <disk_db.hpp>

COMMAND_START(LsSigCommand)
  COMMAND_CLI("ls-sig")
  COMMAND_SHORT_DESCRIPTION("List all the knwon signatures of the internal cache")
  COMMAND_DESCRIPTION(R"(
  list all the knwon signatures of the internal cache.

  See also `help add-sig`, `help rm-sig`.
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    DiskDb("sig").print_list();
    return true;
  }
COMMAND_END()
