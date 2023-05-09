#include <filter.hpp>

FILTER_START(HashFilter)
  FILTER_DESC("hash filter (ls)")
  FILTER_LONG_DESC("check if the hash of the node is equal to arg")
  bool check(const NodeBase & n, const std::string & arg) const override {
    return(arg == n.get_signature().get_hash());
  }
FILTER_END()
