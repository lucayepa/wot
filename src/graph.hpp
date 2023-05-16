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
    for(const auto & h : impl.get_current_set().get()) {
      NodeBase n;
      impl.get(h,n);
      cache[n.get_profile().get_key()].add(h);
    }
  };
  ~GraphView() {};

  Identity get(const IdentityKey & i) const override {
    auto res = Identity(i);
    res.set_nodes(cache.at(i));
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
