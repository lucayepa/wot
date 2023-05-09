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

namespace {
using Hash = std::string;
using JsonString = std::string;
using OrigString = std::string;

using Key = Hash;
using Value = std::pair<OrigString,JsonString>;
}

namespace wot {

// Db of complete nodes, that permits verification or not, based on how
// input was received and accepted. This is all the data we have.
class DbNodes : DbInterface<Key,Value>, ReadonlyDb<Key,NodeBase> {
private:
  // Database implementation of internal json node objects
  DiskDb db;

  // Database implementation of original input, useful for verification of hash
  DiskDb orig_db;

  // Remember how many occurrences of a certain `on` from last visit
  mutable std::map<std::string,int> on_m;
  bool on_needs_update;

  void visit(
    const vm_t & vm,
    bool quiet,
    bool jsonl = false
  ) const;

  // fetch a node from disk, verify it and put it in node object n
  const Node fetch_node(const std::filesystem::directory_entry & file) const;

public:
  DbNodes() : db(), orig_db("orig"), on_needs_update(true) {};
  ~DbNodes() {};

  // fetch a node from disk, and check if it is ok against given filters
  // In order for rule-filter and to-filter to work properly in detailed
  // listing, we should generate an array of nodes reduced by the filters
  bool filter_node(
    const vm_t & vm,
    const std::filesystem::directory_entry & file
  ) const;

  // add a node to the local database
  bool add(
    const Hash & filename,
    const OrigString & orig,
    const JsonString & json
  );

  // add a node to the local database
  bool add (
    const Key & k,
    const Value & v
  ) override {
    return add(k, v.first, v.second);
  }

  // add a node to the local database
  inline bool add(const Node & n) {
    Hash filename = n.get_signature().get_hash();
    return( add(filename, n.get_in(), n.get_json()) );
  }

  // add a node to the local database
  // Apply filters: if node is not ok with all filters, it will not be added
  inline bool add(const Node & n, vm_t vm) {
    if(!n.check_filters(vm)) return false;
    return( add(n) );
  }

  // Interface of ReadonlyDb
  bool get(const Key & k, NodeBase & n) const override;
  void keys(std::set<std::string> &) const override;

  // TODO
  std::optional<Value> get(const Key &) const override { return std::nullopt; };

  // TODO
  bool rm(const Key &) override { return false; };

  void print(const Key & k) const override {
    db.print(k);
  };

  // list all the nodes in the database that match filters defined in vm
  void print_list() const override {
    const vm_t vm{};
    visit(vm, /*quiet=*/false);
  };

  // list all the nodes in the database that match filters defined in vm
  inline void list_nodes(const vm_t & vm) const {
    visit(vm, /*quiet=*/false);
  };

  inline void list_on(const vm_t & vm) const {
    if (on_needs_update) visit(vm, /*quiet=*/true);
    for(const auto & x : on_m) {
      std::cout << x.first << " [" << x.second << "]" << std::endl;
    }
  }

  // export to cout a jsonl file with all the nodes that match vm filters
  //
  // hash and sign of the nodes will not match without the orig files. This
  // means that soon or later we'll need a function `export_all` that gives a
  // file that contains information to verify the nodes
  inline void filtered_export(const vm_t & vm) const {
    visit(vm, /*quiet=*/false, /*jsonl=*/true);
  }
};

} // namespace wot
