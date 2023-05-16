#include <filter.hpp>

FILTER_START(ToFilter)
  FILTER_DESC("to filter")
  FILTER_LONG_DESC("check if there is at least one link that has a field `to` equal to arg")
  bool check(const NodeBase & n, const std::string & arg) const override {
    for(const auto & link : n.get_trust()) {
      if(link.get_to() == arg) {
        return true;
      }
    }
    return false;
  }
FILTER_END()
