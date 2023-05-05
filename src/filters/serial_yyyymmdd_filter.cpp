#include <filter.hpp>

FILTER_START(SerialYyyymmddFilter)
  FILTER_DESCRIPTION("serial is in the form of yyyymmdd filter (ls)")
  FILTER_LONG_DESCRIPTION("check if the serial is in the form yyyymmdd")
  bool check(const NodeBase & n, const std::string & _) const override {
    int serial = n.get_serial();
    // TODO: check days of months, months less than 13 and so on
    return( serial >= 10000000  && serial <= 99991300 );
  }
FILTER_END()
