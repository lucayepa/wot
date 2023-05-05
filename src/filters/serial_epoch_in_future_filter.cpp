#include <string>
#include <ctime>

#include <filter.hpp>

FILTER_START(SerialEpochInFutureFilter)
  FILTER_DESCRIPTION("serial is a Unix epoch integer (seconds) that represent a moment in the past (ls)")
  FILTER_LONG_DESCRIPTION("check if the serial is an integer (seconds) that represent a moment in the past")
  bool check(const NodeBase & n, const std::string & _) const override {
    int serial = n.get_serial();
    std::time_t now = std::time(nullptr);
    return( serial > now );
  }
FILTER_END()
