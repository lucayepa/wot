#include <disk_db.hpp>

#include <fstream>
#include <iostream>
#include <regex>
#include <filesystem>
#include <cstdio>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

  // Not tested on Windows
  std::filesystem::path DiskDb::home_dir() {
    if(_home_dir.has_value()) return _home_dir.value();
    char const* home = getenv("HOME");
    if (home or ((home = getenv("USERPROFILE")))) {
      _home_dir.emplace(std::filesystem::path(home));
    } else {
      char const* hd = getenv("HOMEDRIVE");
      char const* hp = getenv("HOMEPATH");
      if(!hd || !hp) return (std::filesystem::path)"C:";
      _home_dir.emplace((std::string)std::filesystem::path(hd) + (std::string)std::filesystem::path(hp));
    }
    return _home_dir.value();
  }

  bool DiskDb::generic_dir_exists(const std::string & dir) {
    return std::filesystem::is_directory(dir);
  }

  bool DiskDb::generic_mkdir(const std::string & dir) {
   LOG << "Creating directory " << dir;
   try {std::filesystem::create_directory(dir);}
   catch(...) {return false;}
   return true;
  }

  bool DiskDb::generic_mkdir_with_interaction(const std::filesystem::path & dir) {
    if(generic_dir_exists(dir)) return true;
    LOG << "Directory " << dir << " does not exists. Let's create it.";
    if(!generic_mkdir(dir)) {
      std::cerr << "Error while creating dir " << dir << std::endl;
      return false;
    }
    LOG << "Directory created at " << dir;
    return true;
  }

  bool DiskDb::generic_check_or_write_file_with_interaction(
    const std::filesystem::path & abs_dir,
    const std::filesystem::path & abs_file,
    const std::string & content
  ) {
    if(generic_file_exists(abs_file)) return true;
    LOG << "File " << abs_file << " does not exist. Let's create it";
    if(!generic_mkdir_with_interaction(abs_dir)) return false;
    LOG << "Create file at " << abs_file;
    if(!generic_write_file(abs_file,content)) {
      std::cerr << "Error while writing " << abs_file;
      return false;
    }
    if(!generic_file_exists(abs_file)) {
      std::cerr << "Error: file just created does not exists: " << abs_file;
      return false;
    }
    return true;
  }

  bool DiskDb::generic_file_exists(const std::string & file) {
    std::ifstream f(file.c_str());
    return f.good();
  }

  std::stringstream DiskDb::read_file(const std::string & filename) {
    std::ifstream f(DiskDb().get_dir()/filename);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer;
  }

  bool DiskDb::generic_read_file(const std::string & filename, std::string & content) {
    std::ifstream f(filename);
    if(f.fail()) return false;
    std::stringstream buffer;
    LOG << "Reading from " << filename;
    try {
      buffer << f.rdbuf();
      content = buffer.str();
      return true;
    } catch(...) {
      return false;
    }
  }

  bool DiskDb::generic_write_file(const std::string & filename, const std::string & content) {
    std::ofstream f;
    LOG << "Writing to " << filename;
    try {
      f.open(filename);
      f << content;
      f.close();
      return true;
    } catch(...) {
      return false;
    }
  }

  bool DiskDb::generic_remove_file(const std::string & filename) {
    return !std::remove(filename.c_str());
  }

  bool DiskDb::write_file(const std::string & filename, const std::string & content) {
    return DiskDb::generic_write_file(filename, content);
  }

  bool DiskDb::remove_file(const std::string & filename) {
    std::string s = (std::string)(DiskDb().get_dir()/filename);
    LOG << "Removing " << s;
    return !remove(s.c_str());
  }

  bool DiskDb::add(const std::string & hash, const std::string & sig) {
    LOG << "Writing signature to " << hash << ".sig as a known signature";
    return DiskDb::write_file(hash+".sig",sig);
  }

  // remove a known signature from the local database
  bool DiskDb::rm(const std::string & hash) {
    std::string filename = hash + ".sig";
    return remove_file(filename);
  }

  // if there is no file at all, return an empty string
  std::optional<std::string> DiskDb::get(const std::string & hash) const {
    std::stringstream content = DiskDb::read_file(get_ext()=="" ? hash : hash+"."+get_ext());
    if(content.fail()) return std::nullopt;
    return content.str();
  }

  // view a node
  void DiskDb::print(const std::string & hash) const {
    std::stringstream content = DiskDb::read_file(get_ext()=="" ? hash : hash+"."+get_ext());
    if(content.fail()) {
      std::cerr << "File not found!" << std::endl;
    } else {
      std::cout << content.str();
    }
  }

  void DiskDb::print_list() const {
    // list all the known signatures in the database
    std::regex is_sig( get_table() + "$" );
    std::cout << "hash: signature" << std::endl;
    for (const auto & entry : std::filesystem::directory_iterator(DiskDb().get_dir())) {
      if(std::regex_search( (std::string)entry.path(), is_sig )) {
        std::stringstream content = DiskDb::read_file(entry.path().filename());
        std::string filename = entry.path().filename().string();
        std::cout << filename.substr(0, filename.size()-4) << " : " << content.str() << std::endl;
      }
    }
    return;
  }

} // namespace wot