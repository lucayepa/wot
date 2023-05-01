#pragma once

#include <string>

#include <node.hpp>
#include <config.hpp>

namespace {

bool is_bitcoin_address( const std::string & addr ) {
  std::regex is_bitcoin( "^(bc1|[13])[a-zA-HJ-NP-Z0-9]{25,62}$" );
  return std::regex_search( addr, is_bitcoin );
}

} // namespace

namespace wot {

// A wrapper around the identity key
class Identity {
private:
  std::string identity_key;

public:
  Identity(std::string s) : identity_key(s) {};
  Identity(Node n) : identity_key(n.get_profile().get_key()) {};
  ~Identity() = default;

  // Return true if the identity key is "well formed", based on the algo
  // defined in the general config file
  bool is_well_formed() {
    if(Config::get().config["algo"] == "bitcoin") {
      return( is_bitcoin_address(identity_key) );
    } else {
      return false;
    }
  };
};

} // namespace wot
