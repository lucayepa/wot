#include <filter.hpp>

FILTER_START(SerialYyyymmddFilter)
  FILTER_DESC("serial is in the form of yyyymmdd filter (ls)")
  FILTER_LONG_DESC("check if the serial is in the form yyyymmdd")
  FILTER_TOKENS(0)
  bool check(const NodeBase & n) const override {
    int serial = n.get_serial();
    // TODO: check days of months, months less than 13 and so on
    return( serial >= 10000000  && serial <= 99991300 );
  }
FILTER_END()
