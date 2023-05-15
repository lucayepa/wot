#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <config.hpp>
#include <disk_db.hpp>

COMMAND_START(HashObjectCommand)
  COMMAND_CLI("hash-object")
  COMMAND_SYNOPSIS("wot hash-object [FILE]")                            
  COMMAND_SHORT_DESCRIPTION("Calculates the hash of an object")
  COMMAND_DESCRIPTION(R"(
  Calculates the hash of the object contained in the file given as argument.
  If there is no argument, then use stdin.

  See also `help sign`.
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    string s;
    if(vm.count("param")) {
      if(!DiskDb::generic_read_file(vm["param"].as<std::string>(),s))
        return false;
    } else {
      if(!Config::get_input(s)) return false;
    }
    Node n(s);
    n.generate_hash();
    std::cout << n.get_signature().get_hash() << std::endl;
    return true;
  }
COMMAND_END()
