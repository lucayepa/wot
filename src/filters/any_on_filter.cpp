#include <filter.hpp>

FILTER_LINK_START(AnyOnFilter)
  FILTER_DESC("on filter (considered in OR)")
  FILTER_LONG_DESC(
    "check if the link contains at least one `on` that is equal to one of the "
    "provided args")
  FILTER_TOKENS(2)
  bool check(const Link & l, const std::vector<std::string> & v) const override{
    auto f = Config::get().get_filters<Link>()["OnFilter"];
    for(const auto & arg : v){
      if(f->check(l,arg)) return true;
    }
    return false;
  }
FILTER_END()
