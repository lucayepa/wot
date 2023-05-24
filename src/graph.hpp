#pragma once

#include <iostream>

#include <db_nodes.hpp>
#include <identity.hpp>
#include <check_filters.hpp>
#include <vm_t.hpp>

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
  mutable std::map<IdentityKey,MemoryHashSet> cache;
  friend std::ostream & operator<<( std::ostream &, const GraphView & );

  // Database implementation of internal json node objects
  DbNodes impl;

  // Filters used at build-time
  vm_t vm;

  // Completes the graph with identities known only by links. Since we
  // do not have any node of these identities, they will be listed in keys,
  // but hashset value will be empty. Thus, get() will return a shallow identity
  // containing just the key an not the nodes related to it.
  void complete_cache(const NodeBase & n) const {
    for(const auto & l : n.get_trust()) {
      // No link filters used here. All the links enter the cache. Then
      // Identity.get_profiles() and Identity.get_trust() will use them.
      auto to = l.get_to();
      // Add a key with an empty value if we have a "to" of a "link" and we
      // don't have alread a key
      if(cache.count(to)) continue;
      cache[to] = MemoryHashSet();
    }
  }

  void init();

  void colorize( const std::string & badge ) const {
    bool already_colorized = colors.count(badge);
    // No check on the state of db. If called twice with different state of
    // database, strange results can happen.
    if(already_colorized) return;

    po::options_description od = Config::get().get_filters_description();
    // Grab the badge from internal db
    if(!DiskDb("badge").contains(badge))
      throw(std::runtime_error("Badge unknown: " + badge));
    std::string filename = DiskDb("badge").abs_filename(badge);
    vm_t vm;
    po::store(parse_config_file(filename.c_str(), od), vm);
    auto gv = GraphView(vm);
    colors[badge] = gv.keys();
    for (const auto & i : colors[badge]) {
      LOG << "Badge " << badge << " awarded to: " << i;
    }
  }

  using KeySet = std::set<IdentityKey>;
  mutable std::map< std::string, KeySet > colors;

public:

  // Constructor with filters.
  //
  // Node filters are applied at import time (if false, the node does not arrive
  // to cache)
  // Link filters are memorized and used on-the-fly by Identity's get function
  // Identity filters are still not implemented
  //
  // This means that some identity can be in the graph, without any link. If we
  // want to avoid this behaviour, we can check link filters at import time and
  // don't import a node that has all the links checked-false. We cannot remove
  // some link from the nodes, because the graph is only a view of the database,
  // with nodes that remain on the database.
  GraphView( const vm_t & v = vm_t{} ) : impl(), vm(v) {
    init();
  };

  ~GraphView() {};

  // Apply filters does not actually apply anything. It links filters with an
  // identity. Then a call to get_trust() or get_profiles() of the identity,
  // will apply the filters.
  Identity get(const IdentityKey & i) const override {
    auto res = Identity(i);
    res.set_nodes(cache.at(i));
    res.apply_filters(vm);
    return res;
  }

  bool contains(const IdentityKey & i) const override {
    return cache.count(i);
  }

  bool badge(const std::string & badge, const IdentityKey & i) const {
    colorize(badge);
    return colors[badge].count(i);
  }

  KeySet keys() const override {
    std::set<IdentityKey> ks;
    keys(ks);
    return ks;
  }

  void keys(KeySet & ks) const {
    for(auto const & tuple : cache) {
      ks.insert(tuple.first);
    }
  };
};

} // namespace wot
