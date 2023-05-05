#include <identity.hpp>
#include <algo.hpp>

#include <filter.hpp>

FILTER_START(BitcoinKeyFilter)
  FILTER_DESCRIPTION("bitcoin key filter (ls)")
  FILTER_LONG_DESCRIPTION("check if the key is a valid bitcoin address")
  bool check(const NodeBase & n, const std::string & _) const override {
    Identity i(n.get_profile().get_key());
    return(BitcoinAlgo().is_well_formed(i));
  }
FILTER_END()
