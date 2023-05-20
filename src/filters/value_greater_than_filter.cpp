#include <filter.hpp>

FILTER_LINK_START(ValueGreaterThanFilter)
  FILTER_DESC("filter on value greater than the argument")
  FILTER_LONG_DESC("Check if the value of the link is strictly greater than "
  "argument:")
  FILTER_TOKENS(1)

  bool check( const Link & l, const std::string & arg) const override {
    auto value = l.get_value();
    return(value > std::stoi(arg));
  }
FILTER_END()
