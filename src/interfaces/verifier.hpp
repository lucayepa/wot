#pragma once

#include <string>
#include <node.hpp>

using namespace std;
using namespace wot;

// Interface for a program that verifies signed messages
class Verifier {
public:
  virtual ~Verifier() {};

  // Return the CLI that can be used to replicate the call to an external program
  virtual string verify_cli(
    const string & signature,
    const string & address,
    const string & message
  ) = 0;

  virtual bool verify_signature(
    const Node &,
    const string & main_command
  ) = 0;
};
