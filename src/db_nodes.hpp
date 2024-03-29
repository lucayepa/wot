// Db of the nodes we have on disk. It is based on files in a directory
// In relationship has-a with DiskDb.

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>
typedef boost::program_options::variables_map vm_t;

#include <node.hpp>
#include <interfaces/readonly_db.hpp>
#include <disk_db.hpp>
#include <hash_set.hpp>

namespace {
using Hash = std::string;
using JsonString = std::string;
using OrigString = std::string;

// using Value = std::pair<OrigString,JsonString>;
}

namespace wot {

// Db of complete nodes, that permits verification or not, based on how
// input was received and accepted. This is all the data we have.
//
// TODO: interface should have Node as value instead of NodeBase, but filters
// need to get objects of NodeBase.
class DbNodes : public ReadonlyDb<Hash,NodeBase> {
private:
  // Database of internal json node objects
  DiskDb db;

  // Database of original input nodes, useful for verification of hash
  DiskDb orig_db;

  // Remember the current nodes (aka nodes not outdated by other nodes)
  // Index of "current" subset is id-circle
  mutable DiskHashSetWithIndex current;
  mutable bool current_needs_update;

  // Evaluate if the node is current, without using the current index
  bool _is_current(NodeBase & n) const;

  // Cache how many occurrences of a certain `on`
  mutable std::map<std::string,int> on_m;
  mutable bool on_needs_update;

  void visit(
    const std::function< void(std::string) > & f,
    const vm_t & vm
  ) const;

  void update_cache() const;

  // fetch a node from disk, and put it in node object n, without verification
  const Node fetch_node(const std::string & h) const;

public:
  // At the moment current is re-built every time DbNodes is constructed.
  // This means that we can use MemoryHashSet.
  // Maybe in the future, we can initialize current_needs_update to false,
  // provided that we trust that the database is not corrupted.
  DbNodes() : db(), orig_db("orig"), current("cur","curpk"),
    on_needs_update(true), current_needs_update(false) {
  }
  ~DbNodes() { };

  const DiskHashSetWithIndex & get_current_set() const {
    if(current_needs_update) update_cache();
    return current;
  }

  // fetch a node from disk, and check if it is ok against given filters
  // In order for rule-filter and to-filter to work properly in detailed
  // listing, we should generate an array of nodes reduced by the filters
  bool filter_node(
    const vm_t & vm,
    const std::string & h
  ) const;

  // add a node to the local database without filter checking
  bool add(
    const Hash & filename,
    const OrigString & orig,
    const JsonString & json
  );

  // add a node to the local database without filter checking
  inline bool add(const Node & n) {
    Hash filename = n.get_signature().get_hash();
    return( add(filename, n.get_in(), n.get_json()) );
  }

  // add a node to the local database
  // Apply filters: if node is not ok with all filters, it will not be added
  // The node is supposed to pass verify first, and then add.
  inline bool add(const Node & n, vm_t vm) {
    if(!n.check_filters(vm)) return false;
    return( add(n) );
  }

  bool get(const Hash & k, NodeBase & n) const;
  // Existence of key-circle should be checked in advance
  void get_current(const std::string & key_circle, NodeBase & n) const;

  // Interface of ReadonlyDb
  std::set<std::string> keys() const override;

  // TODO
  NodeBase get(const Hash &) const override {return NodeBase(); };

  bool rm(const Hash & h) override {
    NodeBase n;
    if( !get(h,n) ) return false;

    orig_db.rm(h);
    current.reset();

    // A previous hash can come back as current
    current_needs_update = true;
    on_needs_update = true;
    return db.rm(h);
  };

  bool rm(const NodeBase & n) {
    return rm(n.get_signature().get_hash());
  }

  // Evaluate if the node is current, by using the current index
  bool is_current(const NodeBase & n) const {
    return is_current(n.get_signature().get_hash());
  }
  bool is_current(const Hash & h) const;

  void print(const Hash & k) const {
    db.print(k);
  };

  void print_list() const {
    const vm_t vm{};
    list_nodes(std::cout, vm);
  };

  // list all the nodes in the database that match filters defined in vm
  // If the option --jsonl is present, export to cout a jsonl file with all the
  // nodes that match vm filters
  //
  // hash and sign of the nodes will not match without the orig files. This
  // means that soon or later we'll need a function `export_all` that gives a
  // file that contains information to verify the nodes. Until that moment, a
  // tar.gz of the directory ~/.local/share/wot will be ok
  inline void list_nodes(std::ostream & os, const vm_t & vm) const {
    if(vm.count("jsonl")) {
      visit([this](std::string h){print(h);}, vm);
    } else {
      visit(
        [this,&os](std::string h){
          Node n = fetch_node(h);
          n.print_node_summary(os, /*with_links=*/false);
        }, vm
      );
    }
  };

  inline void list_on(const vm_t & vm) const {
    if (on_needs_update) update_cache();
    for(const auto & x : on_m) {
      std::cout << x.first << " [" << x.second << "]" << std::endl;
    }
  }
};

} // namespace wot
