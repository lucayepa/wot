#pragma once

#include <string>

#include <node.hpp>

#include <filter.hpp>

FILTER_START(AlwaysTrueFilter)
  FILTER_DESCRIPTION("dummy filter that returns always true")
  FILTER_LONG_DESCRIPTION("This is a dummy filter that returns always true")
  bool check(const NodeBase & n, const std::string & arg) const override {
    return true;
  }
FILTER_END()
