#include <db_nodes.hpp>

#include <fstream>
#include <regex>

#include <boost/program_options.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace {
  void primary_key_violation_in_db(
    const wot::NodeBase & n1,
    const wot::NodeBase & n2
  ) {
    throw(std::runtime_error(
      (std::string)"Two different nodes with same serial found in db: "+
      n1.get_signature().get_hash() + " " + n2.get_signature().get_hash()
    ));
  };
} // namespace

namespace wot {

// Interface of ReadonlyDb
bool DbNodes::get(const Hash & k, NodeBase & n) const {
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

// Evaluate if a node that is on db is current, without using the index
//
// We suppose that the caller already knows that the node is on db, so we do
// not check it. We just check if it is current or not.
bool DbNodes::_is_current(NodeBase & n) const {
  std::set<std::string> ks;
  keys(ks);
  for(const auto & k : ks ) {
    bool its_me(k == n.get_signature().get_hash());
    if(its_me) { continue; }
    NodeBase node_in_db;
    get(k,node_in_db);
    bool same_key_and_circle =
      n.get_profile().get_key() == node_in_db.get_profile().get_key() &&
      n.get_circle() == node_in_db.get_circle();
    if(same_key_and_circle) {
      if(n.get_serial() == node_in_db.get_serial()) {
        primary_key_violation_in_db(n,node_in_db);
      }
      if(n.get_serial() < node_in_db.get_serial())
        return false;
      // here node_in_db should be flagged for archival
      continue;
    }
  }
  return true;
}

bool DbNodes::is_current(const Hash & h) const {
  update_cache();
  return(current.contains(h));
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

  // The node has been checked and is current.
  current.add(hash);

  // Adding this node can outdate another node, so we do not trust current
  // anymore
  current_needs_update = true;

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

void DbNodes::update_cache() const {
  if(current_needs_update) {
    current.reset();
    std::set<std::string> ks;
    keys(ks);
    for(const auto & h : ks) {
       std::string s;
       db.get(h,s);
       NodeBase n(s);
       if(_is_current(n)) {
         current.add(n.get_signature().get_hash());
       }
    }
    current_needs_update = false;
  }
  if(on_needs_update) {
    for(const auto & h : current.get()) {
      std::string s;
      db.get(h,s);
      NodeBase n(s);
      for(const auto & link : n.get_trust()) {
        for(const auto & r : link.get_on()) { on_m[r]++; }
      }
    }
    on_needs_update = false;
  }
}

void DbNodes::visit(
  const vm_t & vm
) const {
  std::regex is_toml( "orig$" );
  std::regex is_sig( "sig$" );
  auto it = std::filesystem::directory_iterator(db.get_dir());
  for (const auto & entry : it) {
    if(std::regex_search( (std::string)entry.path(), is_toml )) continue;
    if(std::regex_search( (std::string)entry.path(), is_sig )) continue;
    if(DbNodes().filter_node(vm, entry)) {
      LOG << "Found file " << entry.path().filename();
      if(vm.count("jsonl")) {
        std::cout << db.read_file(entry.path().filename()).str() << std::endl;
        continue;
      }
      Node n2 = fetch_node(entry);
      n2.print_node_summary(/*with_links=*/false);
    }
  }
}

} // namespace wot
