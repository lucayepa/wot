#include <signature.hpp>

#include <algo.hpp>

namespace wot {

  bool Signature::is_well_formed() const {
    for(auto & algo : Algos().in_use) {
      if(algo->is_well_formed(*this)) return true;
    }
    return false;
  };

} // namespace wot
