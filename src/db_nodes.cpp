#include <db_nodes.hpp>

#include <fstream>
#include <iostream>
#include <regex>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

  // fetch a node from disk, and check if it is ok against given filters
  // In order for rule-filter and to-filter to work properly in detailed listing, we should generate an array of nodes reduced by the filters
  bool Db_nodes::filter_node(const po::variables_map & vm, const std::filesystem::directory_entry & file) {
    Node n = fetch_node(file);
    //FILTERS are in 'and'
    bool found = true;
    if(vm.count("to-filter")) {
      found = false;
      auto to = vm["to-filter"].as<std::string>();
      for(const auto & link : n.get_trust()) {
        if(link.get_to() == vm["to-filter"].as<std::string>()) {
          found = true;
        }
      }
    }
    if(not found) return false;
    if(vm.count("rule-filter")) {
      found = false;
      auto rule = vm["rule-filter"].as<std::string>();
      for(const auto & link : n.get_trust()) {
        auto rules = link.get_rules();
        if(find(rules.begin(), rules.end(), rule) != rules.end()) {
          found = true;
        }
      }
    }
    if(not found) return false;
    if(vm.count("from-filter")) {
      auto from = vm["from-filter"].as<std::string>();
      found = from == n.get_profile().get_key();
    }
    if(not found) return false;
    if(vm.count("hash-filter")) {
      auto hash = vm["from-filter"].as<std::string>();
      found = hash == n.get_signature().get_hash();
    }
    return true;
  }

  // add a node to the local database
  void Db_nodes::add_node(const std::string & filename, const std::string & orig, const std::string & json) {
    LOG << "Writing " << filename << " having original content: " << orig << " and json content " << json;
    Db::write_file(filename,json);
    if (orig != json) {
      Db::write_file(filename+".orig",orig);
    }
  }

  // fetch a node from disk, verify it and put it in node object n
  const Node Db_nodes::fetch_node(const std::filesystem::directory_entry & file) {
    std::stringstream content = Db::read_file(file.path().filename());
    LOG << "Checking: " << (std::string)file.path().filename();
    Node n2(content.str());
    n2.verify_node(/*force_accpet_hash=*/true,/*force_accpet_sig=*/true);
    return n2;
  }

  // list all the nodes in the database
  void Db_nodes::list_nodes(const po::variables_map & vm) {
    std::regex is_toml( "orig$" );
    std::regex is_sig( "sig$" );
    for (const auto & entry : std::filesystem::directory_iterator(Db().get_dir())) {
      if(std::regex_search( (std::string)entry.path(), is_toml )) continue;
      if(std::regex_search( (std::string)entry.path(), is_sig )) continue;
      if(Db_nodes().filter_node(vm, entry)) {
        LOG << "Found file " << entry.path().filename();
        std::stringstream content = Db::read_file(entry.path().filename());
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
    for (const auto & entry : std::filesystem::directory_iterator(Db().get_dir())) {
      if(std::regex_search( (std::string)entry.path(), is_toml )) continue;
      if(std::regex_search( (std::string)entry.path(), is_sig )) continue;
      std::stringstream content = Db::read_file(entry.path().filename());
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
