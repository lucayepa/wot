#pragma once

#include <string>

#include <node.hpp>
#include <db_nodes.hpp>
#include <vm_t.hpp>

namespace wot {

class GraphView;

// There are multiple node objects related to a single identity, because there
// can be multiple circles for each identity key, and multiple serials for each
// circle. On top of that, every node contains information on one of the
// profiles of its identity. These profile information can be different between
// nodes. Thus, every identity has multiple profiles.
//
// After composing a logic-node from multiple node objects, circle and serial
// go away, and the only remaining information are profiles and trust-links
class Identity {
private:
  std::string identity_key;
  MemoryHashSet nodes;
  bool shallow;
  friend std::ostream & operator<<( std::ostream &, const Identity & );

  vm_t vm;

  const GraphView * context = nullptr;

  std::vector< std::pair<std::string,Link> > backlinks() const;

  // Min of the since fields of the trust vector of the identity
  int earliest_link() const;

public:
  Identity(std::string s) : identity_key(s), shallow(true) {};
  Identity(const NodeBase & n) : identity_key(n.get_profile().get_key()),
    shallow(true) {};
  Identity() : shallow(true) {};
  ~Identity() = default;

  const std::string & get() const { return identity_key; }
  bool is_shallow() const { return shallow; }
  void set_nodes(const MemoryHashSet & m) {
    if(!m.empty()) {
      nodes = m;
      shallow = false;
    }
  }

  void set_context(const GraphView* gv) { context = gv; }
  const GraphView* get_context() const { return context; }

  // Apply (identity) and link filters to the identity. The node filters are
  // skipped, because they are already enforced at node level (the
  // nodes that are in the hashset are already filterd by GraphView).
  //
  // This works on-the-fly by remembering the filters and apply them every time
  // get_trust() is called. Since this can be slow, maybe we need a cache. This
  // would be a design change, because "cache" means that the identity will
  // remember all its data (profiles and trust). This would become difficult in
  // case of big trust vectors.
  //
  // On the other side, if there is a lot of get() calls for the same identity,
  // this method will be very slow.
  void bind_filters(const vm_t & v) { vm = v; }

  // Return true if the identity key is "well formed", based on the algo
  // defined in the general config file
  bool is_well_formed() const;

  // No node filters applied: the semantic of a node filter related to the
  // profile, is that the user does not want any information coming from a node
  // with that profile.
  //
  // So the filters on profiles are applied at node import level.
  void get_profiles(std::vector<Profile> &) const;

  // Filters are applied on-the-fly
  std::vector<Link> get_trust() const;

  // When the identity has first been seen in the context. This means the min
  // between:
  // 1. min of the since fields of trust vector of the identity
  // 2. min of the since fields of the links that points to this identity
  //
  // Link filters should be applied here, so that only acceptable links are
  // taken in consideration (TODO)
  int Identity::seen_since() const;
};

} // namespace wot
