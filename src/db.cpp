#include <db.hpp>

#include <fstream>
#include <iostream>
#include <regex>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

  inline std::stringstream Db::read_file(const std::string & filename) {
    std::ifstream f(Db().get_dir()/filename);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer;
  }

  inline void Db::write_file(const std::string & filename, const std::string & content) {
    std::ofstream f;
    LOG << "Writing to " << (std::string)(Db().get_dir()/filename);
    f.open ((std::string)(Db().get_dir()/filename));
    f << content;
    f.close();
  }

  inline auto remove_file(const std::string & filename) {
    std::string s = (std::string)(Db().get_dir()/filename);
    LOG << "Removing " << s;
    return remove(s.c_str());
  }

  void Db::add(const std::string & hash, const std::string & sig) {
    LOG << "Writing signature to " << hash << ".sig as a known signature";
    Db::write_file(hash+".sig",sig);
  }

  // remove a known signature from the local database
  void Db::rm(const std::string & hash) {
    std::string filename = hash + ".sig";
    remove_file(filename);
  }

  // if there is no file at all, return an empty string
  std::string Db::get(const std::string & hash) {
    std::stringstream content = Db::read_file(get_ext()=="" ? hash : hash+"."+get_ext());
    if(content.fail()) return "";
    return content.str();
  }

  // view a node
  void Db::print(const std::string & hash) {
    std::stringstream content = Db::read_file(get_ext()=="" ? hash : hash+"."+get_ext());
    if(content.fail()) {
      std::cerr << "File not found!" << std::endl;
    } else {
      std::cout << content.str();
    }
  }

  void Db::print_list() {
    // list all the known signatures in the database
    std::regex is_sig( ext + "$" );
    std::cout << "hash: signature" << std::endl;
    for (const auto & entry : std::filesystem::directory_iterator(Db().get_dir())) {
      if(std::regex_search( (std::string)entry.path(), is_sig )) {
        std::stringstream content = Db::read_file(entry.path().filename());
        std::string filename = entry.path().filename().string();
        std::cout << filename.substr(0, filename.size()-4) << " : " << content.str() << std::endl;
      }
    }
    return;
  }

} // namespace wot
