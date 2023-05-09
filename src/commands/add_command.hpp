#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <db_nodes.hpp>
#include <config.hpp>

COMMAND_START(AddCommand)
  COMMAND_CLI("add")
  COMMAND_SHORT_DESCRIPTION("Add a node object to the internal db")
  COMMAND_DESCRIPTION(R"(
  Add a node object to the internal db.
  The object is read from stdin and is verified before of adding it.

  Any filter can be passed on command line. If the node does not pass all the
  filters specified, it will not be added.

  --force-accept-hash do not verify the hash of the node
  --force-accept-sig do not verify the signature of the node
)")

  bool act(const vm_t & vm) const override {
    std::string in;
    if(!Config::get_input(in)) return false;
    Node n(in);
    if( n.verify_node(vm.count("force-accept-hash"),vm.count("force-accept-sig")) ) {
      if(!DbNodes().add(n)) {
        std::cerr << "Node has not been added" << std::endl;
        return false;
      } else {
        return true;
      }
    } else {
      std::cerr << "Node is not valid" << std::endl;
      return false;
    };
  }
COMMAND_END()
