#pragma once

#include <string>
#include <node.hpp>

using namespace std;

// Interface for a program that signs messages
class Signer {
public:
  virtual ~Signer() {};

  virtual string sign_cli(
    const string & wallet_path,
    const string & address,
    const string & message
  ) = 0;

  virtual void suggest_external_sign(
    const string & wallet,
    const wot::Node & n,
    const string & main_command
  ) = 0;

  virtual optional<string> sign(
    const wot::Node & n,
    const string & main_command
  ) = 0;
};
