#include <identity.hpp>

#include <algo.hpp>

namespace wot {

bool Identity::is_well_formed() const {
  for(auto & algo : Algos().in_use) {
    if(algo->is_well_formed(*this)) return true;
  }
  return false;
};

void Identity::get_profiles(std::vector<Profile> & profiles) const {
  if(!nodes_ok) throw(std::logic_error(""));
  for(const auto & h : nodes.get()) {
    NodeBase n;
    DbNodes().get(h,n);
    profiles.push_back(n.get_profile());
  }
}

void Identity::get_trust(std::vector<Link> & links) const {
  if(!nodes_ok) throw(std::logic_error(""));
  for(const auto & h : nodes.get()) {
    NodeBase n;
    DbNodes().get(h,n);
    std::vector<Link> & trust = n.get_mutable_trust();
    std::move(trust.begin(), trust.end(), std::back_inserter(links));
  }
}

std::ostream & operator<<( std::ostream & os, const Identity & i) {
  os << "Identity key: " << i.get() << "\n";

  os << " Nodes:\n" << i.nodes;

  std::vector<Profile> profiles;
  i.get_profiles(profiles);
  int j = 1;
  for(auto const & p : profiles) {
    os << " Profile #" << std::to_string(j++) <<
      ": Name: " <<  p.get_name() << "\n";
  }

  std::vector<Link> links;
  i.get_trust(links);
  j = 1;
  for(auto const & l : links) {
    os << " Link #" << std::to_string(j++) << ": " <<
    " To: " << l.get_to() <<
    " Value: " << std::to_string(l.get_value()) << "\n";
  }
  return os;
}

} // namespace wot
