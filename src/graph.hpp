#pragma once

#include <iostream>

#include <db_nodes.hpp>
#include <identity.hpp>

namespace {
using IdentityKey = std::string;
} // namespace

namespace wot {

class GraphView : public ReadonlyDb<IdentityKey,Identity> {
private:
  std::map<IdentityKey,MemoryHashSet> cache;
  friend std::ostream & operator<<( std::ostream &, const GraphView & );

  // Database implementation of internal json node objects
  DbNodes impl;
public:
  using KeySet = std::set<IdentityKey>;
  GraphView() : impl() {
    for(const auto & h : impl.get_current().get()) {
      NodeBase n;
      impl.get(h,n);
      cache[n.get_profile().get_key()].add(h);
    }
  };
  ~GraphView() {};

  // If key exists, modify passed argument and return true.
  // Otherwise return false.
  bool get(const IdentityKey & _, Identity & i) const override {
    if(cache.count(i.get()) == 0) return false;
    i.set_nodes(cache.at(i.get()));
    return true;
  }

  void keys(std::set<IdentityKey> & ks) const override {
    for(auto const & k : cache) {
      ks.insert(k.first);
    }
  };
};

} // namespace wot
