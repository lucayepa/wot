#include <filter.hpp>

FILTER_START(NoSelfLinkFilter)
  FILTER_DESC("filter that avoid self links")
  FILTER_LONG_DESC("true if node has no self links")
  FILTER_TOKENS(0)
  bool check(const NodeBase & n) const override {
    for(auto const & link : n.get_trust()) {
      if(link.get_to() == n.get_profile().get_key()) {
        return false;
      }
    }
    return true;
  }
FILTER_END()
