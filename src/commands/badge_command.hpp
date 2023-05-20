#pragma once

#include <regex>

#include <boost/program_options.hpp>                                             

#include <nlohmann/json.hpp>

#include <command.hpp>
#include <graph.hpp>
#include <disk_db.hpp>

namespace {
  auto permitted_badge_names = std::regex(R"(^[a-z0-9_-]+$)");
} //namespace

COMMAND_START(BadgeCommand)
  COMMAND_CLI("badge")
  COMMAND_SHORT_DESCRIPTION(
    "Create, remove, or list internal database badges"
  )
  COMMAND_SYNOPSIS(
    "wot [<filter>...] badge [<badgename>]\n"
    "wot (-d | --delete) badge <badgename>\n"
    "wot (-F | --file) <filename> badge <badgename>"
  )
  COMMAND_DESCRIPTION(R"(
  Creates a badge getting the definition from the command line or from a file.

  In order to create a new badge, simply use the same command line filter
  options that can be used with "ls" command, and write "badge <badgename>"
  instead of "ls"

  Badge names can only contain lowercase letters, numbers, underscores and
  dashes.

  -d, --delete
    delete the badge from the system

  -F <filename>, --file <filename>
    load the badge definition from an external file

  If there is no argument, list the badges saved in the internal database.

  See also "help ls"
)")

  static bool sanitize(const std::string & s){
    return std::regex_search(s, permitted_badge_names);
  }

  static std::vector<std::string> argv_to_vector(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    return args;
  }

  // If the badge is defined with a file, the file is saved in the internal db.
  // If the badge is defined from the command line interface, a json array with
  // the argv is saved to intrernal database
  bool act(const vm_t & vm) const override {
    if(!vm.count("param")) {
      DiskDb("badge").print_list();
      return true;
    }
    std::string badgename = vm["param"].as<std::string>();
    if(vm.count("delete")) {
      if( !DiskDb("badge").rm(badgename) ) return false;
      return true;
    }
    if(vm.count("file")) {
      std::string filename = vm["file"].as<std::string>();

      //Check if file is ok with our options description
      po::variables_map vm;
      po::options_description od = Config::get().get_filters_description();
      po::store(parse_config_file(filename.c_str(), od), vm);

      std::string content;
      DiskDb::generic_read_file(filename,content);
      if(content=="") return false;
      DiskDb("badge").add(badgename,content);
      return true;
    }
    int argc = Config::get().get_argc();
    char** argv = Config::get().get_argv();
    auto j = nlohmann::json(argv_to_vector(argc,argv));
    DiskDb("badge").add(badgename,j.dump());
    return true;
  }

  std::pair<bool, std::string> args_ok(const vm_t & vm) const override {
    if(vm.count("delete")) {
      if(vm.size()!=3) {
        return {false, "When deleting a badge, no other options are allowed"};
      }
      return Command::args_ok(vm);
    }
    if(vm.count("file")) {
      if(!vm.count("param")) return {false, "badgename missing"};
    }
    if(!vm.count("param")) return Command::args_ok(vm);
    if(!sanitize(vm["param"].as<std::string>()))
      return {false,
        "Badge name can contain lowercase letters, numbers, '-' and '_'"};
    return Command::args_ok(vm);
  }

  po::options_description cli_options() const override {
    po::options_description od("Badge command options (help badge)");
    od.add_options()
      ("delete,d", "Delete a badge")
      ("file,F", po::value< std::string >(), "Create a badge from file")
    ;
    return od;
  };
COMMAND_END()
