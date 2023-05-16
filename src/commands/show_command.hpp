#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <graph.hpp>

COMMAND_START(ShowCommand)
  COMMAND_CLI("show")
  COMMAND_SHORT_DESCRIPTION("Show the details about an identity")
  COMMAND_SYNOPSIS("wot show <IDENTITY>")
  COMMAND_DESCRIPTION(R"(
  Show a detailed page about the identity given as argument.

  See "help view" to show a page of details about a single node object.
)")

  bool act(const vm_t & vm) const override {
    auto gv = GraphView();
    auto key = vm["param"].as<std::string>();
    if(!gv.contains(key)) return false;
    std::cout << gv.get(key);
    return true;
  }

  std::pair<bool, std::string> args_ok(const vm_t & vm) const override {
    if(vm.count("param")) return Command::args_ok(vm);
    return {false, "Usage: wot show <IDENTITY>"};
  }

COMMAND_END()
