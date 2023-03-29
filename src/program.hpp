// Handling of an external program
#pragma once

#include <string>
#include <fstream>

#include <node.hpp>

using namespace std;
using namespace wot;

// An external program that we call using system call
class Program {
private:
  string name;
  string cli;
  string tmp_filename = "/tmp/result";

  string hint_if_no_executable(const string & action, const Node & n,
      const string & main_command) {

      return( "There is no " + name + " executable on the system. In order "
      "to " + action + " a node, you need " + name + ". If you want to do "
      "it on another system, you can use the following command: \n" + cli +
      "\n\n" +
      "Verify: if the signature is ok, you can add it to the internal db of known "
      "signatures with the following command: \n" +
      main_command + " --signature " + n.get_signature().get_sig() + " add-sig "
      + n.get_signature().get_hash() + "\n" );
  }

  // Return true if the program is on our system
  bool is_present();

public:
  Program(const string & exec_name) : name(exec_name) {};
  ~Program() {};

  // Check if the program is present in the system. If it is not, suggest to
  // cerr how to sign/verify a message on another system.
  bool check_and_suggest_cli(
    const string & action,
    const Node & n,
    const string & main_command
  );

  const string & get_name() const { return name; }

  const string & get_cli() const { return cli; }
  void set_cli(const string & value) { this->cli = value; }

  void set_tmp_filename(const string & value) { this->tmp_filename = value; }

  // Execute the cli
  // Return the system call return value
  int exec(string & output);
};
