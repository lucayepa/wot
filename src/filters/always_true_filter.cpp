#include <filter.hpp>

FILTER_START(AlwaysTrueFilter)
  FILTER_DESC("dummy filter that returns always true")
  FILTER_LONG_DESC("This is a dummy filter that returns always true")
  FILTER_TOKENS(0)
  bool check(const NodeBase & n) const override {
    return true;
  }
FILTER_END()
