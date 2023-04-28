#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <filters.hpp>

namespace {

static std::string header =
R"(# Web of trust configuration file

# This file contains the same options of the command line.
# If an option is used both here and on the command line, the latter prevails.
# Every option is set to its default argument and is commented.
# Feel free to uncomment some lines, change the arguments, and save the file.
# The file can be loaded with the command line option "--config FILE".

#algo = "nostr"
algo = "bitcoin"

signer = "electrum"

verifier = "electrum"

### FILTERS
# The filters are considered in a "AND" relationship. This means that a node
# is compliant with the file if every filter returns true.

)";

} // namespace
COMMAND_START(TemplateConfigFileCommand)
  COMMAND_CLI("template-config-file")
  COMMAND_SHORT_DESCRIPTION("Print a template config file")
  COMMAND_DESCRIPTION(R"(
Generate a template of a config file, with all the possible options. Any
option is set to the default value and is commented.

The idea is that the user can save the file and modify it. Then it can be
used as input of the option "--config FILE". The header
of the file is:
)" + header)

  inline void act_no_ui(std::string & out) const {
    // scan filters and output as options
    out = header;
    for(const auto & f : Filters().all) {
      out += "# " + f->get_description() + "\n";
      out += "# \n";
      out += "# " + f->get_long_description() + "\n";
      out += "#" + f->get_cli_option() + R"( = "arg")" + "\n\n";
    }
  }

  bool act(const boost::program_options::variables_map & vm) const override {
    std::string out;
    act_no_ui(out);
    std::cout << out;
    return true;
  }

COMMAND_END()
