#include <filter.hpp>

FILTER_START(FromFilter)
  FILTER_DESCRIPTION("from filter (ls)")
  FILTER_LONG_DESCRIPTION("check if node key is equal to arg")
  bool check(const NodeBase & n, const std::string & arg) const override {
    return(arg == n.get_profile().get_key());
  }
FILTER_END()
