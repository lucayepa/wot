#include <string>
#include <ctime>

#include <filter.hpp>

FILTER_START(SerialEpochInPastFilter)
  FILTER_DESC("serial is a Unix epoch integer (seconds) that represent a moment in the past (ls)")
  FILTER_LONG_DESC("check if the serial is an integer (seconds) that represent a moment in the past")
  FILTER_TOKENS(0)
  bool check(const NodeBase & n) const override {
    int serial = n.get_serial();
    std::time_t now = std::time(nullptr);
    return( serial < now );
  }
FILTER_END()
