#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <config.hpp>

namespace {

static std::string header =
R"(# Web of trust options file

# This file contains the same options of the command line.
# If an option is used both here and on the command line, the latter prevails.
# In the file, every option is set to its default argument and is commented.
# Feel free to uncomment some lines, change the arguments, and save the file.
# The file can be loaded with the command line option "--options FILE".

### FILTERS
# The filters are considered in a "AND" relationship. This means that a node
# is compliant with the file if every filter returns true.

)";

} // namespace
COMMAND_START(TemplateOptionsFileCommand)
  COMMAND_CLI("template-options-file")
  COMMAND_SHORT_DESCRIPTION("Print a template options file")
  COMMAND_DESCRIPTION(R"(
Generate a template of file with all the possible command line options. Any
option is set to the default value and is commented.

The idea is that the user can save the file and modify it. Then it can be
used as input of the option "--options FILE". The header
of the file is:
)" + header)

  inline void act_no_ui(std::string & out) const {
    // scan filters and output as options
    out = header;
    auto filters = Config::get().get_filters<NodeBase>();
    for(const auto & [k, f] : filters) {
      out += "# " + f->desc() + "\n";
      out += "# \n";
      out += "# " + f->long_desc() + "\n";
      out += "#" + f->cli_option() + R"( = "arg")" + "\n\n";
    }
  }

  bool act(const boost::program_options::variables_map & vm) const override {
    std::string out;
    act_no_ui(out);
    std::cout << out;
    return true;
  }

COMMAND_END()
