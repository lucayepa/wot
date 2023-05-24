#include <vm_t.hpp>

namespace wot {

std::ostream & operator<<( std::ostream & os, vm_t & vm ) {
  os << to_string(vm);
  return os;
}

std::string to_string( const vm_t & vm ) {
  std::string s;
  for(const auto & [k,_] : vm){
    try {
      auto v = vm[k].as<std::string>();
      s += k + " = " + v + "\n";
    } catch(...) { }
    try {
      for(const auto & i : vm[k].as<std::vector<std::string>>()) {
        s += k + " = " + i + "\n";
      }
    } catch(...) { }
    try {
      auto v = vm[k].as<int>();
      s += k + " = " + std::to_string(v) + "\n";
    } catch(...) { }
  }
  return s;
}

} // namespace wot
