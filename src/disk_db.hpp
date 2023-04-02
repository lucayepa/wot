// Basic db on disk based on text files in a directory
// having a certain extension. Filenames, without extension, are keys,
// content of the file is the value

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <config.hpp>
#include <interfaces/db_interface.hpp>

namespace wot {

class DiskDb : public DbInterface {
private:
  std::filesystem::path dir;

public:
  DiskDb(const std::string & ext) : DbInterface(ext) {
    dir = Config::home_dir() / std::string(".local/share/wot");
  };
  DiskDb() {
    dir = Config::home_dir() / std::string(".local/share/wot");
  };
  ~DiskDb() {};

  static bool write_file(const std::string & filename, const std::string & content);
  static std::stringstream read_file(const std::string & filename);
  static bool remove_file(const std::string & filename);

  const std::filesystem::path & get_dir() const { return dir; }
  void set_dir(const std::string & value) { this->dir = value; }

  const std::string & get_ext() const { return get_table(); }

  bool add(const std::string & key, const std::string & value);
  bool rm(const std::string & key);
  std::optional<std::string> get(const std::string & key) const;
  void print(const std::string & hash) const;

  void print_list() const;
};

} // namespace wot
