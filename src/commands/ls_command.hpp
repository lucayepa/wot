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
  List of the identities, filtered with the identity filters provided. Links are
  shown if they match the link filters provided.
)")

  bool act(const vm_t & vm) const override {
    std::cout << GraphView(vm);
    return true;
  }
COMMAND_END()
