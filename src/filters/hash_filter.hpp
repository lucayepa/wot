#include <string>

#include <node.hpp>

#include <filter.hpp>

FILTER_START(HashFilter)
  FILTER_DESCRIPTION("hash filter (ls)")
  FILTER_LONG_DESCRIPTION("check if the hash of the node is equal to arg")
  bool check(const Node & n, const std::string & arg) const override {
    return(arg == n.get_signature().get_hash());
  }
FILTER_END()
