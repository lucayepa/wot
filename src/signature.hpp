#pragma once

#include <node.hpp>
#include <node_qt.hpp>

namespace wot {

class Signature : public wot_qt::Signature {
public:
  Signature(const wot_qt::Signature & s) {
    set_hash(s.get_hash());
    set_sig(s.get_sig());
  }
  Signature(const Node & n) {
    Signature(n.get_signature());
  }
  ~Signature() = default;

  bool is_well_formed() const;
};

} // namespace wot
