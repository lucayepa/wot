#pragma once

#include <string>

#include <node.hpp>
#include <db_nodes.hpp>

namespace wot {

// There will be multiple nodes related to a single identity, because there are
// multiple circles and multiple serials for each circle. Every node will
// contain information on one of the profiles of its identity. These information
// can be different between nodes.
//
// After composing a logic-node from multiple node objects, the only remaining
// information are profiles and trust-links
class Identity {
private:
  std::string identity_key;
  MemoryHashSet nodes;
  bool nodes_ok;
  friend std::ostream & operator<<( std::ostream &, const Identity & );

public:
  Identity(std::string s) : identity_key(s), nodes_ok(false) {};
  Identity(const Node & n) : identity_key(n.get_profile().get_key()),
    nodes_ok(false) {};
  Identity() : nodes_ok(false) {};
  ~Identity() = default;

  const std::string & get() const { return identity_key; }
  void set_nodes(const MemoryHashSet & m) { nodes = m; nodes_ok = true; }

  // Return true if the identity key is "well formed", based on the algo
  // defined in the general config file
  bool is_well_formed() const;

  void get_profiles(std::vector<Profile> &) const;

  std::vector<Link> get_trust() const;

};

} // namespace wot
