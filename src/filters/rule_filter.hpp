#include <string>

#include <node.hpp>

#include <filter.hpp>

FILTER_START(RuleFilter)
  FILTER_DESCRIPTION("rule filter (ls)")
  FILTER_LONG_DESCRIPTION("check if the node contains at least one `link` containing at least one `rule` that is equal to arg")
  bool check(const Node & n, const std::string & arg) const override {
    for(const auto & link : n.get_trust()) {
      auto rules = link.get_rules();
      if(std::find(rules.begin(), rules.end(), arg) != rules.end()) {
        return true;
      }
    }
    return false;
  }
FILTER_END()
