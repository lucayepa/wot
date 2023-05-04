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
#include <disk_db.hpp>

namespace wot {

class Db_nodes {
private:
  DiskDb db;

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
  Db_nodes() : db(), on_needs_update(true) {};
  ~Db_nodes() {};

  // fetch a node from disk, and check if it is ok against given filters
  // In order for rule-filter and to-filter to work properly in detailed
  // listing, we should generate an array of nodes reduced by the filters
  bool filter_node(
    const vm_t & vm,
    const std::filesystem::directory_entry & file
  ) const;

  // add a node to the local database
  bool add_node(
    const std::string & filename,
    const std::string & orig,
    const std::string & json
  );

  // add a node to the local database
  inline bool add_node(const Node & n) {
    std::string filename = n.get_signature().get_hash();
    return( add_node(filename, n.get_in(), n.get_json()) );
  }

  // add a node to the local database
  // Apply filters: if node is not ok with all filters, it will not be added
  inline bool add_node(const Node & n, vm_t vm) {
    if(!n.check_filters(vm)) return false;
    return( add_node(n) );
  }

  // list all the nodes in the database that are selected by vm filters
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
