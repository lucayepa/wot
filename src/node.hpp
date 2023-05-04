// A node enhanced with implementation info (toml or json)
// and with all the methods that depend on implementation
// (siging, hashing, ...)
#pragma once

#include <string>
#include <fstream>

#include <boost/program_options.hpp>

#include <toml.hpp>

#include <node_qt.hpp>

namespace wot {

typedef boost::program_options::variables_map vm_t;

using wot_qt::Profile, wot_qt::Link;

// Operations on node that don't need to know anything about source/hash/sig and
// similar stuff. This is for managing nodes that come from internal db, so they
// are already trusted, and are on disk as json objects. This is supposed to be
// fast.
//
// Objects of this class do not contain enough information to verify.
//
// Hash is not supposed to be correct, because maybe it is related to the hash
// of an original document.
//
// Signature is correct, because it is based on profile.key and hash
class NodeBase : public wot_qt::Node {
private:
  void print_link_summary(const Link & l) const;

public:
  NodeBase(const std::string & jsons);
  NodeBase() = default; // TODO: delete?
  ~NodeBase() = default;

  // Check if the node is ok with all the filters provided. The filters are
  // evaluated in "AND" and are short circuited.
  bool check_filters(const vm_t & vm) const;

  std::string to_j(const bool withsig) const;
  std::string get_json() const { return to_j(/*withsig=*/true); }

  void print_node_summary(bool with_links) const;
};

// It contains the input as a string, because some functions need the input. The
// input is held in a string and is the original form needed to calculate hash
// and to verify signature
class Node : public NodeBase {
private:
  const std::string in;
  bool origin_is_toml;

  /*
   In order to do the same from CLI:
   vi /tmp/test.json
   tr --delete '\n' < /tmp/test.json | sha256sum
  */
  std::string hash_calc() const;

  // Can be done in C++, but we mimic the user via CLI
  static std::string hash_calc(const std::string & toml);

  // Check the object format
  bool pre_parse(toml::table &);

  bool verify_hash() const;

  // Mimic the CLI from a user
  bool input_hash_verify_toml() const;
  bool node_hash_verify() const;

  void sign();
  void generate_hash();

public:
  //Node(const Node & n) : NodeBase(n.in), in{n.in} {};
  Node(const std::string s);
  Node() = default; // TODO: delete?
  ~Node() = default;

  // Import from another node only the core node content, not in
  //void import(const wot_qt::Node & n);

  const std::string & get_in() const { return in; }

  static void solve( std::string_view in, toml::table & t );

  // Check some fields of the node. Useful before of system calls.
  bool sanitize() const;

  bool signature_verify() const;

  // Generate hash and signature for a node
  //
  // If `in` is json, return a valid and signed json node, else, sign the toml
  // object, and return it signed.
  //
  // Return a string with a json or toml signed node if signature was correctly
  // generated. Throw if the user has been informed on what are the next steps.
  std::string get_signed(bool force_accept_hash);

  // General node verification
  // Return true if it is a valid node
  bool verify_node(
    const bool force_accept_hash,
    const bool force_accept_sig ) const;
};

} // namespace wot
