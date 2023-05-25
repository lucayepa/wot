#include <iostream>
#include <graph.hpp>

namespace wot {

void GraphView::init() {
  LOG << "Creating graph using vm: " << to_string(vm);
  // First iteration: build graph based on filters that do not need context
  for(const auto & h : impl.get_current_set().get()) {
    LOG << "1: Evaluating node " << h;
    NodeBase n;
    impl.get(h,n);
    if(!check_filters(vm,n)) continue;
    LOG << "1: Node is ok. Adding to cache: " << h;
    cache[n.get_profile().get_key()].add(h);
  }
  // Second iteration: erase some nodes using filters that need context
  // This iteration wil be done again and again until nothing is removed.
  while(true) {
    LOG << "Iteration with: " << to_string(vm);
    bool something_removed = false;
    for(const auto & [i,hs] : cache){
      auto set = hs.get();
      LOG << "2: Evaluating identity key " << i << " with " << set.size() <<
        " hashes";
      MemoryHashSet new_set;
      for(const auto & h : set) {
        LOG << "2: Evaluating hash " << i << "->" << h;
        NodeBase n;
        impl.get(h,n);
        if(check_filters(vm,n,this)) {
          LOG << "2: Node is ok. Adding it: " << i << "->" << h;
          new_set.insert(h);
        } else {
          LOG << "2: Node is not ok. Removing from cache: " << i << "->" << h;
          something_removed = true;
        }
      }
      if(new_set.empty()) {
        cache.erase(i);
      } else {
        cache[i]=new_set;
      }
    }
    if(!something_removed) break;
  }
  // Third iteration: add shallow identities to the remaining graph. Pay
  // attention: if you use this in a badge definition graph, shallow
  // identities will receive the badge
  if(!vm.count("all-identities") || !vm["all-identities"].as<bool>()) return;
  for(const auto & [i,hs] : cache){
    auto set = hs.get();
    for(const auto & h : set) {
      NodeBase n;
      impl.get(h,n);
      complete_cache(n);
    }
  }
}

// TODO output the HashSet using operator of HashSet
// TODO: use cache to visit only once, like:
// for(auto const & [k,v] : g.cache) {
//   os << k << ": " << &v << std::endl;
// }
std::ostream & operator<<( std::ostream & os, const GraphView & g) {
  for(auto const & k : g.keys()) {
    os << g.get(k);
  }
  return os;
}

} // namespace wot
