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

  // Add shallow identities to the graph.
  //
  // Completes the graph by adding identities known only by links. Since we
  // do not have any node of these identities, they will be listed by keys(),
  // but hashset value will be empty. Thus, get() will return a shallow identity
  // containing just the key an not the nodes related to it.
  //
  // Actually, we can have nodes of these identities on db, but they are
  // filtered by node filters. This mixes up and generates confusion. For
  // example, a filter that checks if we have a profile for a certain identity
  // returns false. This is why, as a general rule, node filters should be
  // applied only at add time.
  //
  // In order to apply link filters, identities are created. This slows down the
  // initialization phase
  LOG << "Iteration 2 with: " << to_string(vm);
  KeySet shallow_keys;
  for(const auto & [k,_] : key_index){
    LOG << "2: Evaluating identity " << k << " with " << _.get().size() <<
      " hashes";
    auto i = get(k);
    for(const auto & l : i.get_trust()) {
      auto to = l.get_to();
      // Add a key with an empty value if we have a "to" of a "link" and we
      // don't have alread a key
      if(key_index.count(to)) continue;
      LOG << "2: Shallow identity " << to << "is ok. Adding it.";
      shallow_keys.insert(to);
    }
  }
  for(const auto & k : shallow_keys) {
    key_index[k] = MemoryHashSet();
  }

  // Apply the identity filters and stage the identities that match the identity
  // filters.
  //
  // Link filters will be applied inside the get() function, when identities are
  // created.
  //
  // It generates all the identities, so it can be slow.
  for(auto const & [i,_] : key_index) {
    if(check_filters(vm,get(i))) {
      LOG << "3: Identity " << i << " is ok. Staging it.";
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
