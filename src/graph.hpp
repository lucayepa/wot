#pragma once

#include <iostream>

#include <db_nodes.hpp>
#include <identity.hpp>
#include <config.hpp>
#include <check_filters.hpp>
#include <vm_t.hpp>

namespace {
using IdentityKey = std::string;
} // namespace

namespace wot {

// A container of Identities.
//
// It contains a fixed set of keys, and generates values (Identity objects)
// on-the-fly when there is a get request.
//
// The object is built from db, based on some filters given to constructor.
// Nodes that do not pass the filters, are not included in the container.
//
// Identity filters are applied and keys that are ok with the filters are
// staged. So the identity filters can see both the original key set and the
// staged keys.
//
// Link filters are not applied by the constructor.
//
// Shallow identities (identities that are not in db, but are pointed by
// identities that are in db) are added to the object at the end of construction
// time. This means that some shallow identities are not supposed to be there,
// if they are pointed by a link that was supposed to be filtered out.
//
// By design this class does not contain any information on the links, but
// gather every information from the database of nodes. This is useful with
// large datasets.
class GraphView : public ReadonlyDb<IdentityKey,Identity> {
private:
  using KeySet = std::set<IdentityKey>;

  mutable KeySet staged;

  mutable std::map<IdentityKey,MemoryHashSet> key_index;

  friend std::ostream & operator<<( std::ostream &, const GraphView & );

  // Database implementation of internal json node objects
  DbNodes impl;

  // Filters used at build-time
  vm_t vm;

  void stage(IdentityKey k) const { staged.insert(k); }

  void init();

  // Assign a badge
  //
  // A new GraphView is created with the vm gathered from the file of the badge.
  // The identities that are staged in the new GraphView just created are the
  // ones that should have the badge.
  void colorize( const std::string & badge ) const {
    bool already_colorized = colors.count(badge);
    // No check on the state of db. If called twice with different state of
    // database, strange results can happen.
    if(already_colorized) return;

    po::options_description od = Config::get().get_filters_description();
    // Read the badge from internal db
    if(!DiskDb("badge").contains(badge))
      throw(std::runtime_error("Badge unknown: " + badge));
    std::string filename = DiskDb("badge").abs_filename(badge);
    vm_t vm;
    po::store(parse_config_file(filename.c_str(), od), vm);
    auto gv = GraphView(vm);
    colors[badge] = gv.staged_keys();
    for (const auto & i : colors[badge]) {
      LOG << "Badge " << badge << " awarded to: " << i;
    }
  }

  mutable std::map< std::string, KeySet > colors;

public:

  // Constructor with filters.
  //
  // Three type of filters:
  // Node filters: applied at import time (if false, the node will not be
  // included)
  // Link filters: memorized and used on-the-fly by Identity's get function
  // Identity filters: applied when we ask details on profiles or trust vector
  // to an identity
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

  // Filters are binded to the identity, but they are not applied. A call to
  // get_trust() or get_profiles() of the identity, will check the filters.
  //
  // Caller knows that object exists. For example, by calling contains().
  Identity get(const IdentityKey & i) const override {
    auto res = Identity(i);
    res.set_nodes(key_index.at(i));
    res.bind_filters(vm);
    res.set_context(this);
    return res;
  }

  bool contains(const IdentityKey & i) const override {
    return key_index.count(i);
  }

  bool badge(const std::string & badge, const IdentityKey & i) const {
    colorize(badge);
    return colors[badge].count(i);
  }

  const KeySet & staged_keys() const { return staged; }

  KeySet keys() const override {
    std::set<IdentityKey> ks;
    keys(ks);
    return ks;
  }

  void keys(KeySet & ks) const {
    for(auto const & [key,_] : key_index) {
      ks.insert(key);
    }
  };
};

} // namespace wot
