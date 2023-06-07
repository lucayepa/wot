#include <iostream>

#include <graph.hpp>
#include <check_filters.hpp>

namespace wot {

void GraphView::init() {
  LOG << "Creating graph using vm: " << to_string(vm);

  // First iteration: evaluate nodes
  for(const auto & h : impl.get_current_set().get()) {
    LOG << "1: Evaluating node " << h;
    NodeBase n;
    impl.get(h,n);
    // Node filters are applied here
    if(!check_filters(vm,n)) continue;
    LOG << "1: Node is ok. Adding it: " << h;
    key_index[n.get_profile().get_key()].add(h);
  }

        " hashes";
      for(const auto & h : set) {
        NodeBase n;
        impl.get(h,n);
        //
        if(check_filters(vm,n,this)) {

        } else {

        }
      }
    }
  }
*/

  // Add shallow identities to the remaining graph (if requested)
  //
  // Beware: if you use this in a badge definition graph, shallow
  // identities will receive the badge
  if(vm.count("all-identities") && vm["all-identities"].as<bool>()) {
    for(const auto & [i,hs] : key_index){
      auto set = hs.get();
      for(const auto & h : set) {
        NodeBase n;
        impl.get(h,n);
        complete_graph(n);
      }
    }
  }

  // Now the graph is complete. Apply the identity filters and stage
  // the identities that are ok with the identity filters. Link filters will be
  // used inside the function when identities are created.
  //
  // This generate all the identities and can be slow
  //
  // Since identity is generated, link filters are applied to the newly
  // generated identity, and then identity filters are considered.
  for(auto const & [i,_] : key_index) {
    if(check_filters(vm,get(i))) {
      LOG << "2: Identity " << i << " is ok. Staging it.";
      stage(i);
    }
  }
}

// TODO output the HashSet using operator of HashSet
// TODO: use key_index to visit only once, like:
// for(auto const & [k,v] : g.key_index) {
//   os << k << ": " << &v << std::endl;
// }
std::ostream & operator<<( std::ostream & os, const GraphView & g) {
  for(auto const & k : g.staged_keys()) {
    os << g.get(k);
  }
  return os;
}

} // namespace wot
