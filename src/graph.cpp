#include <iostream>
#include <graph.hpp>

namespace wot {

// TODO output the HashSet using operator of HashSet
// TODO: use cache to visit only once, like:
// for(auto const & [k,v] : g.cache) {
//   os << k << ": " << &v << std::endl;
// }
std::ostream & operator<<( std::ostream & os, const GraphView & g) {
  os << "Identities\n";
  for(auto const & k : g.keys()) {
    os << g.get(k);
  }
  return os;
}

} // namespace wot
