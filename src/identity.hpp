#pragma once

#include <string>

#include <node.hpp>
#include <db_nodes.hpp>

namespace wot {

// There will be multiple nodes related to a single identity, because there are
// multiple circles and multiple serials for each circle. On top of that, every
// node contains information on one of the profiles of its identity. These
// information can be different between nodes. So every identity has multiple
// profiles.
//
// After composing a logic-node from multiple node objects, the only remaining
// information are profiles and trust-links
class Identity {
private:
  std::string identity_key;
  MemoryHashSet nodes;
  bool nodes_ok;
  friend std::ostream & operator<<( std::ostream &, const Identity & );

  vm_t vm;

public:
  Identity(std::string s) : identity_key(s), nodes_ok(false) {};
  Identity(const NodeBase & n) : identity_key(n.get_profile().get_key()),
    nodes_ok(false) {};
  Identity() : nodes_ok(false) {};
  ~Identity() = default;

  const std::string & get() const { return identity_key; }
  void set_nodes(const MemoryHashSet & m) { nodes = m; nodes_ok = true; }

  // Apply identity and link filters to the identity. The node filters are
  // skipped, because they are already enforced at node level (the
  // nodes that are in the hashset are already filterd by GraphView).
  //
  // This works on-the-fly by remember the filters and apply them every time
  // get_trust is called. This can be slow, so maybe we need a
  // cache. This would be a design change, because "cache" means that the
  // identity will remember all its data (profiles and trust). This would
  // become difficult in case of big trust vectors.
  void apply_filters(const vm_t & v) { vm = v; }

  // Return true if the identity key is "well formed", based on the algo
  // defined in the general config file
  bool is_well_formed() const;

  // No node filters applied: the semantic of a filter on the profile is that
  // the user does not want any information coming from a node with that
  // profile,
  //
  // So the filters on profiles are applied at node import level.
  void get_profiles(std::vector<Profile> &) const;

  // Filters are applied on-the-fly
  std::vector<Link> get_trust() const;

};

} // namespace wot
