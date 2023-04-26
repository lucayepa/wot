#pragma once

#include <string>

#include <node.hpp>

#include <filter.hpp>
#include <filters/hash_filter.hpp>
#include <filters/from_filter.hpp>
#include <filters/to_filter.hpp>
#include <filters/rule_filter.hpp>

FILTER_START(GrepFilter)
  FILTER_DESCRIPTION("grep filter (ls)")
  FILTER_LONG_DESCRIPTION("check if the arg is somehow present in the node")
  bool check(const Node & n, const std::string & arg) const override {
    return(
      HashFilter().check(n,arg) ||
      FromFilter().check(n,arg) ||
      ToFilter().check(n,arg) ||
      RuleFilter().check(n,arg)
    );
  }
FILTER_END()
