#include <filter.hpp>
#include <db_nodes.hpp>

FILTER_START(NewSerialFilter)
  FILTER_DESC("node has a new key (identity.circle.serial) unknown to us")
  FILTER_LONG_DESC(
    "node has a new primary key (identity.circle.serial)"
  )
  FILTER_TOKENS(0)

  bool check(const NodeBase & n) const override {
    std::set<std::string> ks;
    DbNodes().keys(ks);
    for(const auto & k : ks ) {
      NodeBase node_in_db;
      DbNodes().get(k,node_in_db);
      if(
        n.get_profile().get_key() == node_in_db.get_profile().get_key() &&
        n.get_circle() == node_in_db.get_circle() &&
        n.get_serial() <= node_in_db.get_serial()
      ) return false;
    }
    return true;
  }
FILTER_END()