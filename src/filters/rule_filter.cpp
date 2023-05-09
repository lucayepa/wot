#include <filter.hpp>

FILTER_START(RuleFilter)
  FILTER_DESC("rule filter (ls)")
  FILTER_LONG_DESC("check if the node contains at least one `link` containing at least one `rule` that is equal to arg")
  bool check(const NodeBase & n, const std::string & arg) const override {
    for(const auto & link : n.get_trust()) {
      auto on = link.get_on();
      if(std::find(on.begin(), on.end(), arg) != on.end()) {
        return true;
      }
    }
    return false;
  }
FILTER_END()
