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
Generate a template of file with all the possible command line options. All the
options are set to the default value and are commented.

The idea is that the user can save the file and modify it. Then it can be
used as input of the option "--options FILE". The header
of the file is:
)" + header)

  template<class T>
  void print_filters(std::ostream & os) const {
    auto filters = Config::get().get_filters<T>();
    for(const auto & [k, f] : filters) {
      os << "# " + f->desc() + "\n"
         << "# \n"
         << "# " + f->long_desc() + "\n";
      switch(f->tokens()) {
        case 1:
          os << "#" + f->cli_option() + R"( = "arg")" + "\n";
          break;
        case 0:
          os << "#" + f->cli_option() + "\n";
          break;
        default:
          os << "#" + f->cli_option() + R"( = "arg1")" + "\n";
          os << "#" + f->cli_option() + R"( = "arg2")" + "\n";
      }
      os << "\n";
    }
  }

  void print_content(std::ostream & os) const {
    // scan filters and output as options
    os << header;

    os << "######\n";
    os << "# Node filters (use with add, ls-nodes or ls)\n";
    os << "######\n";
    print_filters<NodeBase>(os);

    os << "######\n";
    os << "# Link filters (use with ls)\n";
    os << "######\n";
    print_filters<Link>(os);
  }

  bool act(const boost::program_options::variables_map & vm) const override {
    print_content(std::cout);
    return true;
  }

COMMAND_END()
