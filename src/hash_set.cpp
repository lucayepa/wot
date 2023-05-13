#include <hash_set.hpp>

namespace wot {

std::ostream & operator<<( std::ostream & os, const HashSet<std::string> & hs) {
  for (auto const & h : hs.get()) {
    os << h << "\n";
  }
  return os;
}

} // namespace wot
