#include <identity.hpp>

#include <algo.hpp>

namespace wot {

bool Identity::is_well_formed() const {
  for(auto & algo : Algos().in_use) {
    if(algo->is_well_formed(*this)) return true;
  }
  return false;
};

} // namespace wot
