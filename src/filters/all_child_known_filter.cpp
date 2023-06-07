#include <iostream>

#include <filter.hpp>

#include <graph.hpp>

FILTER_IDENTITY_START(AllChildKnownFilter)
  FILTER_DESC("every linked identity has at least one node in internal db")
  FILTER_LONG_DESC("check if all the keys linked from this node are present in "
    "our database")
  FILTER_TOKENS(0)
  bool check(const Identity & i) const override {
    for(auto const & child : i.get_trust()) {
      if(!i.get_context()->contains(child.get_to())) {
        return false;
      }
    }
    return(true);
  }
FILTER_END()
