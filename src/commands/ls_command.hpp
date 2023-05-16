#pragma once

#include <command.hpp>
#include <graph.hpp>

COMMAND_START(LsCommand)
  COMMAND_CLI("ls")
  COMMAND_SHORT_DESCRIPTION(
    "List of the identities that match the requested filters."
  )
  COMMAND_SYNOPSIS(
    "wot [FILTERS ...] ls"
  )
  COMMAND_DESCRIPTION(R"(
  List of the identities, filtered with the filters provided.
)")

  bool act(const vm_t & vm) const override {
    std::set<std::string> identities;
    GraphView().keys(identities);
    for(auto const & i : identities) {
      std::cout << i << std::endl;
    }
    return true;
  }
COMMAND_END()
