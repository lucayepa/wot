#pragma once

#include <string>
#include <sstream>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <config.hpp>

COMMAND_START(SolveCommand)
  COMMAND_CLI("solve")
  COMMAND_SHORT_DESCRIPTION("")
  COMMAND_DESCRIPTION("")

  inline void solve_no_ui(const std::string & in, std::string & out) const {
    toml::table t;
    Node::solve( (std::string_view)(in.c_str()), t );
    std::stringstream ss; ss << t << std::endl;
    out = ss.str();
  }

  bool act(const boost::program_options::variables_map & vm) const override {
    std::string s;
    if(!Config::get_input(s)) return false;
    std::string solved;
    solve_no_ui(s, solved);
    std::cout << solved;
    return true;
  }

  bool hidden() const override { return true; }
COMMAND_END()
