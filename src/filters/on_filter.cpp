#include <filter.hpp>

FILTER_LINK_START(OnFilter)
  FILTER_DESC("on filter")
  FILTER_LONG_DESC("check if the node contains at least one `link` containing at least one `on` that is equal to arg")
  FILTER_TOKENS(1)
  bool check(const Link & l, const std::string & arg) const override {
    auto on = l.get_on();
    if(std::find(on.begin(), on.end(), arg) != on.end()) {
      return true;
    }
    return false;
  }
FILTER_END()
