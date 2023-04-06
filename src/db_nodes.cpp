#include <db_nodes.hpp>

#include <fstream>
#include <iostream>
#include <regex>

#include <filters.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

  // fetch a node from disk, and check if it is ok against given filters
  // In order for rule-filter and to-filter to work properly in detailed
  // listing, we should generate an array of nodes reduced by the filters
  bool Db_nodes::filter_node(const po::variables_map & vm, const std::filesystem::directory_entry & file) {
    Node n = fetch_node(file);

    // All the filters are evaluated in "and" mode
    for(const auto & f : Filters().all) {
      if(vm.count(f->get_name()) && !f->check(n,vm[f->get_name()].as<std::string>()) ) return false;
      LOG << f->get_name();
    }
    return true;
  }

  // add a node to the local database
  void Db_nodes::add_node(const std::string & filename, const std::string & orig, const std::string & json) {
    LOG << "Writing " << filename << " having original content: " << orig << " and json content " << json;
    DiskDb::write_file(filename,json);
    if (orig != json) {
      DiskDb::write_file(filename+".orig",orig);
    }
  }

  // fetch a node from disk, verify it and put it in node object n
  const Node Db_nodes::fetch_node(const std::filesystem::directory_entry & file) {
    std::stringstream content = DiskDb::read_file(file.path().filename());
    Node n2(content.str());
    n2.verify_node(/*force_accpet_hash=*/true,/*force_accpet_sig=*/true);
    return n2;
  }

  // list all the nodes in the database
  void Db_nodes::list_nodes(const po::variables_map & vm) {
    std::regex is_toml( "orig$" );
    std::regex is_sig( "sig$" );
    for (const auto & entry : std::filesystem::directory_iterator(DiskDb().get_dir())) {
      if(std::regex_search( (std::string)entry.path(), is_toml )) continue;
      if(std::regex_search( (std::string)entry.path(), is_sig )) continue;
      if(Db_nodes().filter_node(vm, entry)) {
        LOG << "Found file " << entry.path().filename();
        std::stringstream content = DiskDb::read_file(entry.path().filename());
        Node n2(content.str());
        // Since they are already in our db, do not verify hash and signature
        n2.verify_node(/*force_accpet_hash=*/true,/*force_accpet_sig=*/true);
        n2.print_node_summary(/*with_links=*/false);
      }
    }
  }

  void Db_nodes::list_rules() {
    std::regex is_toml( "orig$" );
    std::regex is_sig( "sig$" );
    for (const auto & entry : std::filesystem::directory_iterator(DiskDb().get_dir())) {
      if(std::regex_search( (std::string)entry.path(), is_toml )) continue;
      if(std::regex_search( (std::string)entry.path(), is_sig )) continue;
      std::stringstream content = DiskDb::read_file(entry.path().filename());
      Node n;
      LOG << "Checking: " << (std::string)entry.path().filename();
      Node n2(content.str());
      n2.verify_node(/*force_accpet_hash=*/true,/*force_accpet_sig=*/true);
      std::map<std::string,int> rules;
      for(const auto & link : n2.get_trust()) {
        for(const auto & rule : link.get_rules()) {
          rules[rule]++;
        }
      }
      for(const auto & x : rules) {
        std::cout << x.first << " [" << x.second << "]" << std::endl;
      }
    }
  }


} // namespace wot
