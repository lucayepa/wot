#include <filter.hpp>

FILTER_LINK_START(ToFilter)
  FILTER_DESC("to filter")
  FILTER_LONG_DESC("checks if the `to` field of the link is equal to arg")
  FILTER_TOKENS(1)
  bool check(const Link & l, const std::string & arg) const override {
    return (l.get_to() == arg);
  }
FILTER_END()
