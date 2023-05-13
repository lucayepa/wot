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
  std::set<std::string> identity_keys;
  g.keys(identity_keys);
  for(auto const & k : identity_keys) {
    Identity i(k);
    g.get("",i);
    os << i;
  }
  return os;
}

} // namespace wot
