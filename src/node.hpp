// A node enhanced with implementation info (toml or json)
// and with all the methods that depend on implementation
// (siging, hashing, ...)
#pragma once

#include <string>
#include <fstream>

#include <toml.hpp>

#include <node_qt.hpp>

namespace wot {

using wot_qt::Profile, wot_qt::Link;

// Common interface that hides if something is made on json or toml
// It contains both the input as a string and a node, because some
// functions need the input. The input is held in strings
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
  std::string hash_calc() const;

  // Can be done in C++, but we mimic the user via CLI
  static std::string hash_calc(const std::string & toml);

  std::string to_j(const bool withsig) const;

  // Check the object format, and transform toml -> json (if needed)
  // Populate json_s with a valid json in any case, and throw if not possible
  bool pre_parse();
  // Populate the node core information based on json_s coming from pre_parse
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
  // sign, or verify, action is needed. For example:
  // Node n(s); n.verify_node(true,true);
  Node(const std::string s) : in(s) {};
  Node() = default;
  ~Node() = default;

  // Import from another node only the core node content, not json_s, nor in
  void import(const wot_qt::Node & n);

  void print_node_summary(bool with_links) const;

  const std::string & get_json() const { return json_s; }
  const std::string & get_in() const { return in; }

  static void solve( std::string_view in, toml::table & t );

  // Check some fields of the node. Useful before of system calls.
  bool sanitize() const;

  bool signature_verify() const;

  // Generate hash and signature for a node
  //
  // Since in can be json or toml, the caller is supposed to know already what
  // is the format, and set the argument `as_toml` accordingly.
  //
  // If in is json (as_toml == false), return a valid and signed json node
  // else, sign the toml object, and return both a toml object and a json node
  // with hash and signature that are NOT valid for json, but are taken from
  // the toml object.
  //
  // Populate the core node information with a valid Node structure
  //
  // Populate json_s with a valid json (having the wrong hash if the input was
  // toml, like explained above)
  //
  // Return a string with a json or toml signed node if signature was correctly
  // generated, nullopt if the user has been informed on what are the next
  // steps. In this case json_s can contain an half-ready artifact.
  std::optional<std::string> get_signed(const bool as_toml, const bool force_accept_hash);

  // General node verification and generation of json_s
  // Modify json_s
  // Return a valid Node structure in n
  // Return a valid json in json_s (having the wrong hash if the input was a toml)
  // Return true if it is a valid node
  bool verify_node(const bool force_accept_hash, const bool force_accept_sig );

};

} // namespace wot
