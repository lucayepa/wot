// Basic db on disk based on text files in a directory
// having a certain extension. Filenames without extension are keys,
// content of the file is the value

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <config.hpp>

namespace wot {

class Db {
private:
  std::filesystem::path dir;
  std::string ext;

public:
  Db(const std::string & ext) : ext(ext) {
    dir = Config::home_dir() / std::string(".local/share/wot");
  };
  Db() : ext("") {
    dir = Config::home_dir() / std::string(".local/share/wot");
  };
  ~Db() {};

  static void write_file(const std::string & filename, const std::string & content);
  static std::stringstream read_file(const std::string & filename);

  const std::filesystem::path & get_dir() const { return dir; }
  void set_dir(const std::string & value) { this->dir = value; }

  const std::string & get_ext() const { return ext; }
  void set_ext(const std::string & value) { this->ext = value; }

  void add(const std::string & key, const std::string & value);
  void rm(const std::string & key);
  std::string get(const std::string & key);
  void print(const std::string & hash);

  void print_list();
};

} // namespace wot
