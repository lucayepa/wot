#include <filter.hpp>
#include <db_nodes.hpp>

// This is the same of NewSerialFilter, with the difference that, if the
// node is present in the db, NewSerialFilter returns always false.
// On the other side, this filter returns true if two conditions are both true:
// 1. the node is on db
// 2. it would be accepted in case it would be evaluated without being already
//    in the db
//
// In case of ls, this filter works as expected. In case of add, it returns
// false unless we have a copy of the same object in db, AND it is the current
// one (the one with the highest serial).
// If this filter returns false, the node should be flagged for garbage
// collection or archival.
FILTER_START(IsCurrentFilter)
  FILTER_DESC("node is the newset for its (identity.circle)")
  FILTER_LONG_DESC(
    "node is the newest we have on db for its identity key and circle"
  )
  FILTER_TOKENS(0)

  bool check(const NodeBase & n) const override {
    auto dbn = DbNodes();
    if( !dbn.contains(n.get_signature().get_hash()) ) return false;
    return(dbn.is_current(n));
  }
FILTER_END()
