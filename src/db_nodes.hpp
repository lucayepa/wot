// Db of the nodes we have on disk. It is based on files in a directory
// In relationship has-a with DiskDb.

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <node.hpp>
#include <disk_db.hpp>

namespace wot {

class Db_nodes {
private:
  DiskDb db;

  // Remember how many occurrences of a certain `on` from last visit
  mutable std::map<std::string,int> on_m;
  bool on_needs_update;

  // ostream equal to clog means no print
  void visit(
    const po::variables_map & vm,
    bool quiet
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
    const po::variables_map & vm,
    const std::filesystem::directory_entry & file
  ) const;

  // add a node to the local database
  void add_node(
    const std::string & filename,
    const std::string & orig,
    const std::string & json
  );

  // list all the nodes in the database that are selected by vm filters
  inline void list_nodes(const po::variables_map & vm) const {
    visit(vm, /*quiet=*/false);
  };

  inline void list_on(const po::variables_map & vm) const {
    if (on_needs_update) visit(vm, /*quiet=*/true);
    for(const auto & x : on_m) {
      std::cout << x.first << " [" << x.second << "]" << std::endl;
    }
  }

};

} // namespace wot
