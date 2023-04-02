#include <disk_db.hpp>

#include <fstream>
#include <iostream>
#include <regex>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

  inline std::stringstream DiskDb::read_file(const std::string & filename) {
    std::ifstream f(DiskDb().get_dir()/filename);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer;
  }

  inline bool DiskDb::write_file(const std::string & filename, const std::string & content) {
    std::ofstream f;
    LOG << "Writing to " << (std::string)(DiskDb().get_dir()/filename);
    try {
      f.open ((std::string)(DiskDb().get_dir()/filename));
      f << content;
      f.close();
      return true;
    } catch(...) {
      return false;
    }
  }

  inline bool DiskDb::remove_file(const std::string & filename) {
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
