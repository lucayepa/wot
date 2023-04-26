#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <config.hpp>

COMMAND_START(SolveCommand)
  COMMAND_CLI("solve")
  COMMAND_SHORT_DESCRIPTION("")
  COMMAND_DESCRIPTION("")

  bool act(const boost::program_options::variables_map & vm) const override {
    string s;
    if(!Config::get_input(s)) return false;
    toml::table t;
    Node::solve( (std::string_view)(s.c_str()), t );
    std::cout << t << std::endl;
    return true;
  }

  bool hidden() const override { return true; }
COMMAND_END()
