#include <filter.hpp>

FILTER_START(ToFilter)
  FILTER_DESC("to filter")
  FILTER_TOKENS(2)
  FILTER_LONG_DESC(
    "check if there is at least one link that has a field `to` equal to arg. "
    "This filter can be used more than once. In this case the nodes having at "
    "least one link to every key pass the filter"
  )

  bool has_link(const NodeBase & n, const std::string & to) const {
    for(const auto & link : n.get_trust()) {
      if(link.get_to() == to) {
        return true;
      }
    }
    return false;
  }

  bool check(
    const NodeBase & n,
    const std::vector<std::string> & v
  ) const override {
    for(auto const & to : v) {
      if(!has_link(n,to)) return false;
    }
    return true;
  }
FILTER_END()
