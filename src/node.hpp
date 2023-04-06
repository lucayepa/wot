// A node enhanced with implementation info (toml or json)
// and with all the methods that depend on implementation
// (siging, hashing, ...)
#pragma once

#include <string>
#include <fstream>

#include <toml.hpp>

#include <node_qt.hpp>

namespace wot {

using wot_qt::Profile, wot_qt::Signature, wot_qt::Sources, wot_qt::Link;

// Common interface that hides if something is made on json or toml
// It contains both the input as a string and a node, because some
// functions need the input. The input is hold in a JsonView or
// TomlView enhanced strings
class Node : public wot_qt::Node {
private:
  const std::string in;
  std::string json_s;
  // Node n;

  /*
   In order to do the same from CLI:
   vi /tmp/test.json
   tr --delete '\n' < /tmp/test.json | sha256sum
  */
  const std::string hash_calc() const;

  // Can be done in C++, but we mimic the user via CLI
  static std::string hash_calc(const std::string & toml);

  const std::string to_j(const bool withsig) const;

  static bool is_bitcoin_address( const std::string & addr ) {
    std::regex is_bitcoin( "^(bc1|[13])[a-zA-HJ-NP-Z0-9]{25,62}$" );
    return std::regex_search( addr, is_bitcoin );
  }

  static bool is_base64( const std::string & sig ) {
    std::regex base64( "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$" );
    return std::regex_search( sig, base64 );
  }

  // Check the object format, and transform toml -> json (if needed)
  // Populate json_s with a valid json in any case, and throw if not possible
  bool pre_parse();
  // Populate n based on json_s coming from pre_parse
  bool generate_node();

  bool verify_hash();

  // Mimic the CLI from a user
  bool input_hash_verify_toml() const;
  bool node_hash_verify() const;

  void print_link_summary(const Link & l) const;

public:
  Node(const Node & n) : in(n.in), json_s(n.json_s) {
    import(n);
  };
  // Creating a Node with the constructor does not populate the internal node
  // content. In order to have a fully functional Node from a toml file, a
  // sign, or verify, action is needed:
  // Node n(s); n.verify_node(true,true);
  Node(const std::string s) : in(s) {};
  Node() = default;
  ~Node() = default;

  // Import from another node only the basic node content, not json_s
  void import(const wot_qt::Node & n);

  void print_node_summary(bool with_links) const;

  const std::string & get_json() const { return json_s; }
  // const Node & get_node() const { return n; }

  static void solve( std::string_view in, toml::table & t );

  // Check some fields of the node. Useful before of system calls.
  bool sanitize() const;

  bool signature_verify() const;

  // Generate hash and signature for a node
  //
  // in is the input that can be json or toml
  //
  // the caller is supposed to know already what it is, and set as_toml accordingly
  //
  // if the input is json (as_toml == false), return a valid and signed json node
  // else, sign the toml object, and return both a toml object and a json node
  // with hash and signature that are NOT valid for json, but are taken from
  // the toml object.
  //
  // Populate n with a valid Node structure
  // Populate json_s with a valid json (having the wrong hash if the input was a toml)
  // Return a string with json or toml signed node if signature was correctly generated,
  // nullopt if the user has been informed on next steps. In this case json_s
  // can be an half-ready artifact.
  std::optional<std::string> get_signed(const bool as_toml, const bool force_accept_hash);

  // General node verification and generation of json_s
  // Modify json_s
  // Return a valid Node structure in n
  // Return a valid json in s (having the wrong hash if the input was a toml)
  // Return true if it is a valid node
  bool verify_node(const bool force_accept_hash, const bool force_accept_sig );

};

} // namespace wot
