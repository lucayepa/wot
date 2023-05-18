#include <filter.hpp>

FILTER_START(GrepFilter)
  FILTER_DESC("grep filter")
  FILTER_LONG_DESC("check if the arg is somehow present in the node")
  bool check(const NodeBase & n, const std::string & arg) const override {
    auto f = Config::get().get_filters<NodeBase>();
    return(
      f["HashFilter"]->check(n,arg) ||
      f["FromFilter"]->check(n,arg)
    );
  }
FILTER_END()
