#pragma once

#include <iostream>

#include <db_nodes.hpp>
#include <identity.hpp>

namespace {
using IdentityKey = std::string;
} // namespace

namespace wot {

// A container of Identities that contains a fixed set of keys, and generates
// Identity objects on-the-fly when there is a get request.
//
// By design this class does not contain any information on the graph, but
// gather every information from the database of nodes. This is useful with
// large datasets.
class GraphView : public ReadonlyDb<IdentityKey,Identity> {
private:
  std::map<IdentityKey,MemoryHashSet> cache;
  friend std::ostream & operator<<( std::ostream &, const GraphView & );

  // Database implementation of internal json node objects
  DbNodes impl;

  // Filters used at build-time
  vm_t vm;
public:
  using KeySet = std::set<IdentityKey>;

  // Constructor with filters.
  //
  // Node filters are applied at import time (if false do not arrive to cache)
  // Link filters are memorized and used on-the-fly by Identity's get function
  // Identity filters are still not implemented
  //
  // This means that some identity can be in the graph, without any link. If we
  // want to avoid this behaviour, we can check link filters at import time and
  // don't import a node that has all the links checked-false. We cannot remove
  // some link from the nodes, because the graph is only a view of the database,
  // with nodes that remain on the database.
  //
  // At the moment the information on vm comes from config.
  GraphView( const vm_t & v = vm_t{} ) : impl(), vm(v) {
    for(const auto & h : impl.get_current_set().get()) {
      NodeBase n;
      impl.get(h,n);
      if(!n.check_filters(vm)) continue;
      cache[n.get_profile().get_key()].add(h);
    }
  };

  ~GraphView() {};

  Identity get(const IdentityKey & i) const override {
    auto res = Identity(i);
    res.set_nodes(cache.at(i));
    res.apply_filters(vm);
    return res;
  }

  bool contains(const IdentityKey & i) const override {
    return cache.count(i);
  }

  std::set<IdentityKey> keys() const override {
    std::set<IdentityKey> ks;
    keys(ks);
    return ks;
  }

  void keys(std::set<IdentityKey> & ks) const {
    for(auto const & k : cache) {
      ks.insert(k.first);
    }
  };
};

} // namespace wot
