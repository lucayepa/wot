#include <filter.hpp>

FILTER_START(ContainsToFilter)
  FILTER_DESC("node contains at least one link to a certain key")
  FILTER_TOKENS(2)
  FILTER_LONG_DESC(
    "check if there is at least one link that has a field `to` equal to arg. "
    "This filter can be used more than once. In this case the nodes pass the "
    "filter if they have at least one link to every key"
  )

  bool has_link(const NodeBase & n, const std::string & to) const {
    auto f = Config::get().get_filters<Link>()["ToFilter"];
    for(const auto & link : n.get_trust()) {
      if(f->check(link,to)) return true;
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
