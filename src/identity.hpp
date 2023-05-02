#pragma once

#include <string>

#include <node.hpp>

namespace wot {

// There will be multiple nodes related to a single identity, because there are
// multiple circles and multiple serials for each circle. Every node will have
// information on the profile of its identity. These information can be
// different between identities.
class Identity {
private:
  std::string identity_key;

public:
  Identity(std::string s) : identity_key(s) {};
  Identity(const Node & n) : identity_key(n.get_profile().get_key()) {};
  ~Identity() = default;

  const std::string & get() const { return identity_key; }

  // Return true if the identity key is "well formed", based on the algo
  // defined in the general config file
  bool is_well_formed() const;
};

} // namespace wot
