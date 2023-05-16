#include <identity.hpp>

#include <algo.hpp>
#include <nlohmann/json.hpp>

namespace wot {

bool Identity::is_well_formed() const {
  for(auto & algo : Algos().in_use) {
    if(algo->is_well_formed(*this)) return true;
  }
  return false;
};

void Identity::get_profiles(std::vector<Profile> & profiles) const {
  if(!nodes_ok) throw(std::logic_error(""));
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
  if(!nodes_ok) throw(std::logic_error(""));
  std::vector<Link> links;
  for(const auto & h : nodes.get()) {
    NodeBase n;
    DbNodes().get(h,n);
    std::vector<Link> & trust = n.get_mutable_trust();
    std::move(trust.begin(), trust.end(), std::back_inserter(links));
  }
  return links;
}

std::ostream & operator<<( std::ostream & os, const Identity & i) {
  os << "Identity key: " << i.get() << "\n";

  os << " Nodes:\n" << i.nodes;

  os << " Profiles:\n";

  std::vector<Profile> profiles;
  i.get_profiles(profiles);

  int j = 1;
  for(auto const & p : profiles) {
    os << " #" << std::to_string(j++) << p;
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
