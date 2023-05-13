#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <config.hpp>

COMMAND_START(VerifyCommand)
  COMMAND_CLI("verify")
  COMMAND_SHORT_DESCRIPTION("Verify a node object")
  COMMAND_DESCRIPTION(R"(
  Get a node from stdin and verify it.

  This means to apply the default checks that are used to accept a node in the
  database by the "add" command. The three checks are:
  * hash is the correct hash of the node
  * signature is verified for the hash and the identity provided
  * if the database already contains another node with the same "identity" and
    "circle", the "serial" field should be strictly greater than the one on the
    node that is already in the database

  Experts only:

  These three checks can be skipped with the following options:
  --force-accept-hash - do not check the hash of the object
  --force-accept-sig - do not check the signature of the object
  --force-no-db-check - do not use internal database to verify the object
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    string s;
    if(!Config::get_input(s)) return false;
    Node n(s);
    if( n.verify_node(vm) ) {
      std::cout << "ok" << std::endl;
      return true;
    } else {
      std::cout << "NOT ok" << std::endl;
      return false;
    }
  }
COMMAND_END()
