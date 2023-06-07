#include <iostream>

#include <filter.hpp>

#include <graph.hpp>

FILTER_IDENTITY_START(AnyChildKnownIdentityFilter)
  FILTER_DESC("at least one of the linked identity is present in internal db")
  FILTER_LONG_DESC("check if at least one identity linked from this one is "
    "present in our database")
  FILTER_TOKENS(0)
  bool check(const Identity & i) const override {
    for(auto const & child : i.get_trust()) {
      if(i.get_context()->contains(child.get_to())) {
        return true;
      }
    }
    return(false);
  }
FILTER_END()
