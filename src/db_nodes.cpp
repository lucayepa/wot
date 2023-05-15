#include <db_nodes.hpp>

#include <fstream>
#include <regex>
#include <functional>

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

std::set<std::string> DbNodes::keys() const {
  return db.keys();
}

// Evaluate if a node that is on db is current, without using the index
//
// We suppose that the caller already knows that the node is on db, so we do
// not check it. We just check if it is current or not.
bool DbNodes::_is_current(NodeBase & n) const {
  for(const auto & k : keys() ) {
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
  const std::string & h
) const {
  Node n = fetch_node(h);
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
  const std::string & hash
) const {
  std::string s;
  db.get(hash,s);
  Node n2(s);
  // Since the nmode is already in our db, do not verify hash and signature
  // We assume that every needed check is already done at add time
  return n2;
}

void DbNodes::update_cache() const {
  if(current_needs_update) {
    current.reset();
    for(const auto & h : keys()) {
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
  const std::function <void (std::string)> & f,
  const vm_t & vm
) const {
  for(const auto h : keys()) {
    if(filter_node(vm, h)) {
      LOG << "Found file " << h;
      f(h);
    }
  }
}

} // namespace wot
