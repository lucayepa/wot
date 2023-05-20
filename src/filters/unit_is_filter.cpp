#include <filter.hpp>

FILTER_LINK_START(UnitIsFilter)
  FILTER_DESC("unit filter")
  FILTER_LONG_DESC("Check if the link unit is equal to arg. It accepts multiple"
  " arguments. The link is considered valid if the unit is equal to one of the "
  "arguments provided.")
  FILTER_TOKENS(2)

  bool check(
    const Link & l,
    const std::vector<std::string> & arg
  ) const override {
    auto unit = l.get_unit();
    if(std::find(arg.begin(), arg.end(), unit) != arg.end()) {
      return true;
    }
    return false;
  }
FILTER_END()
