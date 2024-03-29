// Basic db on disk based on text files in a directory
// having a certain extension. Filenames, without extension, are keys,
// content of the file is the value

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <optional>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

#include <interfaces/db.hpp>

namespace wot {

class DiskDb : public Db<std::string,std::string> {
private:
  std::filesystem::path dir;

  // Cache of the home_dir information
  std::optional<std::filesystem::path> _home_dir;

public:
  DiskDb(const std::string & ext) : Db(ext) {
    dir = home_dir() / std::string(".local/share/wot");
    generic_mkdir_with_interaction(dir);
  };
  DiskDb() {
    dir = home_dir() / std::string(".local/share/wot");
    generic_mkdir_with_interaction(dir);
  };
  ~DiskDb() {};

  // Generic functions walid for any file. Return true on success.
  static bool generic_file_exists(const std::string & file);
  static bool generic_dir_exists(const std::string & dir);
  static bool generic_mkdir(const std::string & dir);
  static bool generic_write_file(
    const std::string & filename,
    const std::string & content
  );
  static bool generic_remove_file(const std::string & filename);
  static bool generic_read_file(
    const std::string & filename,
    std::string & content
  );

  // Wrap around generic_mkdir, with LOG and cerr
  // Does not create parents
  static bool generic_mkdir_with_interaction(const std::filesystem::path & dir);
  // Wrap around generic_write_file, with LOG and cerr
  static bool generic_check_or_write_file_with_interaction(
    const std::filesystem::path & abs_dir,
    const std::filesystem::path & abs_file,
    const std::string & content
  );

  std::filesystem::path home_dir();

  static bool write_file(
    const std::string & filename,
    const std::string & content
  );
  static std::stringstream read_file(const std::string & filename);
  static bool remove_file(const std::string & filename);

  const std::filesystem::path & get_dir() const { return dir; }
  void set_dir(const std::string & value) { this->dir = value; }

  const std::string get_ext() const { return get_database_name(); }

  bool add(const std::string & key, const std::string & value) override;
  bool rm(const std::string & key) override;
  std::string get(const std::string & key) const override;

  bool get(const std::string & key, std::string & value) const {
    if(contains(key)) {
      value = get(key);
      return true;
    }
    return false;
  }
  void keys(std::set<std::string> &) const override;
  std::set<std::string> keys() const override;
  void print(const std::string & hash) const;

  // Return the absolute filename of a certain key. This is useful to read the
  // file directly from other libraries that will not use our interface
  // The existence of the key should be checked beforehand.
  std::string abs_filename(const std::string & k) const;

  void print_list() const;
};

} // namespace wot
