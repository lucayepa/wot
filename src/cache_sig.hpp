// Db of the cache of signatures we have on disk. It is based on files in a directory
// In relationship has-a with db.

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

class Cache_sig {
private:
  std::filesystem::path dir;
  std::string ext;
  DiskDb db;

public:
  Cache_sig() : db("sig") {};
  ~Cache_sig() {};

  static bool signature_verify_from_cache(const Node n);
};

} // namespace wot
