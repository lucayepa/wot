// Db of the nodes we have on disk. It is based on files in a directory
// In relationship has-a with db.

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <node.hpp>

#include <db.hpp>

namespace wot {

  using nlohmann::json;

class Db_nodes {
private:
  std::filesystem::path dir;
  std::string ext;
  Db db;

  static const Node fetch_node(const std::filesystem::directory_entry & file);

public:
  Db_nodes() : db() {};
  ~Db_nodes() {};

  static bool filter_node(const po::variables_map & vm, const std::filesystem::directory_entry & file);
  static void add_node(const std::string & filename, const std::string & orig, const std::string & json);
  static void list_nodes(const po::variables_map & vm);
  static void list_rules();

};

} // namespace wot
