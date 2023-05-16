#pragma once

#include <command.hpp>
#include <db_nodes.hpp>

COMMAND_START(LsNodesCommand)
  COMMAND_CLI("ls-nodes")
  COMMAND_SHORT_DESCRIPTION(
    "List of the node objects in the internal db that match the requested "
    "filters."
  )
  COMMAND_SYNOPSIS(
    "wot [--jsonl] [FILTERS ...] ls-nodes"
  )
  COMMAND_DESCRIPTION(R"(
  List of the objects of the internal db, filtered with the filters provided.

  If option "--jsonl" is present, the list will be printed to stdout in a jsonl
  way (one json object per line). This can be used to export the whole db.

  The objects exported can have a hash or signature that is of the original
  form, so the db can be not-verifiable.

  Link filters: the objects are first filtered, then are listed as full objects.
  This means that if a single link of an object pass a link filter, then all the
  links are shown, because the whole object is shown.
)")

  bool act(const vm_t & vm) const override {
    DbNodes().list_nodes(std::cout, vm);
    return true;
  }
COMMAND_END()
