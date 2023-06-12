#include <identity.hpp>

#include <iostream>
#include <node.hpp>
#include <check_filters.hpp>
#include <algo.hpp>
#include <nlohmann/json.hpp>

#include <graph.hpp>

namespace wot {

int Identity::earliest_link() const {
  if(shallow) return INT_MAX;
  int min = INT_MAX; // This assumes that "since" is signed
  for(auto const & link : get_trust()) {
    if(link.get_since() < min) {
      min = link.get_since();
    }
  }
  // Identities without any links will return INT_MAX
  return min;
}

int Identity::seen_since() const {
  if(context == nullptr) throw(std::logic_error("Context not found"));

  int min_received = INT_MAX; // This assumes that "since" is signed
  for(auto const & [from, link] : backlinks()) {
    if(link.get_since() < min_received) {
      min_received = link.get_since();
    }
  }

  int min_sent = earliest_link();
  if(min_received < min_sent) return min_received;
  return min_sent;
}

std::vector< std::pair<std::string,Link> > Identity::backlinks() const {
    std::vector< std::pair<std::string,Link> > result;
    for(auto const & k : context->keys()) {
      auto from_identity = context->get(k);
      for(auto const & link : from_identity.get_trust()) {
        if(link.get_to() == identity_key) {
          std::pair<IdentityKey,Link> pair(k,link);
          result.push_back(pair);
        }
      }
    }
    return result;
}

bool Identity::is_well_formed() const {
  for(auto & algo : Algos().in_use) {
    if(algo->is_well_formed(*this)) return true;
  }
  return false;
};

void Identity::get_profiles(std::vector<Profile> & profiles) const {
  if(shallow) throw(std::logic_error("No profiles for a shallow identity"));
  // Use a set of JSON strings to enforce uniqueness
  std::set<std::string> json_set;
  auto dbn = DbNodes();
  for(const auto & h : nodes.get()) {
    NodeBase n;
    dbn.get(h,n);
    nlohmann::json json;
    wot_qt::to_json(json, n.get_profile());
    json_set.insert(json.dump());
  }
  for(const auto & jsons : json_set) {
    auto json = nlohmann::json::parse(jsons);
    Profile p;
    wot_qt::from_json(json, p);
    profiles.push_back(p);
  }
}

std::vector<Link> Identity::get_trust() const {
  if(shallow) throw(std::logic_error("A shallow identity has no trust vector"));
  std::vector<Link> links;
  for(const auto & h : nodes.get()) {
    NodeBase n;
    DbNodes().get(h,n);
    std::vector<Link> & trust = n.get_mutable_trust();
    apply_filters(vm,trust);
    std::move(trust.begin(), trust.end(), std::back_inserter(links));
  }
  return links;
}

std::ostream & operator<<( std::ostream & os, const Identity & i) {
  os << "Identity: " << i.get() << (i.shallow ? " (shallow)" : "") << "\n";

  // If not properly loaded from a graph, we don't have a logic node, so we
  // show only the key
  if(i.shallow) { return os;}

  os << " Nodes:\n" << i.nodes;

  os << " Profiles:\n";

  std::vector<Profile> profiles;
  i.get_profiles(profiles);

  int j = 1;
  for(auto const & p : profiles) {
    os << " #" << std::to_string(j++) << ": " << p;
  }

  os << " Links:\n";

  std::vector<Link> links = i.get_trust();
  j = 1;
  for(auto const & l : links) {
    os << " #" << std::to_string(j++) << ": " << l;
  }
  return os;
}

} // namespace wot
