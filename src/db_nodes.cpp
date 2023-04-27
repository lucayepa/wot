#include <db_nodes.hpp>

#include <fstream>
#include <regex>

#include <filters.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

bool Db_nodes::filter_node(
  const po::variables_map & vm,
  const std::filesystem::directory_entry & file
) const {
  Node n = fetch_node(file);

  // All the filters are evaluated in "and" mode
  for(const auto & f : Filters().all) {
    const std::string name = f->get_name();
    if( vm.count(name) && !f->check(n,vm[name].as<std::string>()) )
      return false;
    LOG << name << " passed";
  }
  return true;
}

void Db_nodes::add_node(
  const std::string & filename,
  const std::string & orig,
  const std::string & json
) {
  LOG << "Writing " << filename << " having original content: " << orig <<
    " and json content " << json;
  db.write_file(filename,json);
  if (orig != json) {
    db.write_file(filename+".orig",orig);
  }
  on_needs_update = true;
}

const Node Db_nodes::fetch_node(
  const std::filesystem::directory_entry & file
) const {
  std::stringstream content = db.read_file(file.path().filename());
  Node n2(content.str());
  // Since they are already in our db, do not verify hash and signature
  // We assume that checks are done at add time
  n2.verify_node(/*force_accpet_hash=*/true,/*force_accpet_sig=*/true);
  return n2;
}

void Db_nodes::visit(
  const po::variables_map & vm,
  bool quiet
) const {
  std::regex is_toml( "orig$" );
  std::regex is_sig( "sig$" );
  auto it = std::filesystem::directory_iterator(db.get_dir());
  for (const auto & entry : it) {
    if(std::regex_search( (std::string)entry.path(), is_toml )) continue;
    if(std::regex_search( (std::string)entry.path(), is_sig )) continue;
    if(Db_nodes().filter_node(vm, entry)) {
      LOG << "Found file " << entry.path().filename();
      Node n2 = fetch_node(entry);
      for(const auto & link : n2.get_trust()) {
        for(const auto & r : link.get_on()) { on_m[r]++; }
      }
      if(!quiet) n2.print_node_summary(/*with_links=*/false);
    }
  }
}

} // namespace wot
