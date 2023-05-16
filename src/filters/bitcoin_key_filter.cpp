#include <identity.hpp>
#include <algo.hpp>

#include <filter.hpp>

FILTER_START(BitcoinKeyFilter)
  FILTER_DESC("bitcoin key filter")
  FILTER_LONG_DESC("check if the key is a valid bitcoin address")
  FILTER_TOKENS(0)
  bool check(const NodeBase & n) const override {
    Identity i(n.get_profile().get_key());
    return(BitcoinAlgo().is_well_formed(i));
  }
FILTER_END()
