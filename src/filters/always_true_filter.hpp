#include <string>

#include <node.hpp>

#include <filter.hpp>

FILTER_START(AlwaysTrueFilter)
  FILTER_DESCRIPTION("dummy filter that returns always true")
  FILTER_LONG_DESCRIPTION("No long description needed")
  bool check(const Node & n, const std::string & arg) const override {
    return true;
  }
FILTER_END()
