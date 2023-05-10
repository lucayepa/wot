#include <db_nodes.hpp>

#include <fstream>
#include <regex>

#include <boost/program_options.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

// Interface of ReadonlyDb
bool DbNodes::get(const Key & k, NodeBase & n) const {
  std::optional<std::string> result = db.get(k);
  if(result.has_value()) {
    auto j = nlohmann::json::parse(result.value());
    from_json(j, n);
    return true;
  } else {
    return false;
  }
}

void DbNodes::keys(std::set<std::string> & ks) const {
  db.keys(ks);
}

bool DbNodes::filter_node(
  const vm_t & vm,
  const std::filesystem::directory_entry & file
) const {
  Node n = fetch_node(file);
  return n.check_filters(vm);
}

bool DbNodes::add(
  const std::string & hash,
  const std::string & orig,
  const std::string & json
) {
  LOG << "Writing " << hash << " having original content: " << orig <<
    " and json content " << json;
  if(!db.add(hash,json)) return false;
  if (orig != json) {
    if(!orig_db.add(hash,orig)) return false;
  }
  on_needs_update = true;
  return true;
}

const Node DbNodes::fetch_node(
  const std::filesystem::directory_entry & file
) const {
  std::stringstream content = db.read_file(file.path().filename());
  Node n2(content.str());
  // Since they are already in our db, do not verify hash and signature
  // We assume that checks are done at add time
  return n2;
}

void DbNodes::visit(
  const vm_t & vm,
  bool quiet,
  bool jsonl
) const {
  std::regex is_toml( "orig$" );
  std::regex is_sig( "sig$" );
  auto it = std::filesystem::directory_iterator(db.get_dir());
  for (const auto & entry : it) {
    if(std::regex_search( (std::string)entry.path(), is_toml )) continue;
    if(std::regex_search( (std::string)entry.path(), is_sig )) continue;
    if(DbNodes().filter_node(vm, entry)) {
      LOG << "Found file " << entry.path().filename();
      if(jsonl) {
        std::cout << db.read_file(entry.path().filename()).str();
      } else {
        Node n2 = fetch_node(entry);
        for(const auto & link : n2.get_trust()) {
          for(const auto & r : link.get_on()) { on_m[r]++; }
        }
        if(!quiet) n2.print_node_summary(/*with_links=*/false);
      }
    }
  }
}

} // namespace wot
