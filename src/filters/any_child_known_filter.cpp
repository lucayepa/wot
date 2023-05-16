#include <iostream>

#include <filter.hpp>

#include <graph.hpp>

FILTER_START(AnyChildKnownFilter)
  FILTER_DESC("at least one of the linked identity is present with a node in internal db")
  FILTER_LONG_DESC("check wether a key linked from this node is present in our database")
  FILTER_TOKENS(0)
  bool check(const NodeBase & n) const override {
    auto gv = GraphView();
    for(auto const & child : n.get_trust()) {
      if(gv.contains(child.get_to())) {
        return true;
      }
    }
    return(false);
  }
FILTER_END()