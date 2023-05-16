#include <filter.hpp>
#include <db_nodes.hpp>

FILTER_START(NewSerialFilter)
  FILTER_DESC("node has a key (identity.circle.serial) unknown to us")
  FILTER_LONG_DESC(
    "node has a new primary key (identity.circle.serial)"
  )
  FILTER_TOKENS(0)

  bool check(const NodeBase & n) const override {
    auto dbn = DbNodes();
    std::string key = n.get_profile().get_key() + '-' + n.get_circle();
    if( !dbn.get_current_set().index_contains(key) ) return true;
    NodeBase node_in_db;
    dbn.get_current(key,node_in_db);
    if( n.get_serial() <= node_in_db.get_serial() ) return false;
    return true;
  }
FILTER_END()
