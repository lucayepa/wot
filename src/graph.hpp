#pragma once

#include <node.hpp>
#include <db_nodes.hpp>

namespace {
using Hash = std::string;
using K = Hash;
} // namespace

namespace wot {

class GraphView : public ReadonlyDb<K,NodeBase> {
private:
  // Database implementation of internal json node objects
  DbNodes impl;

public:
  GraphView() : impl() {};
  ~GraphView() {};

  // If key exists, modify passed NodeBase and return true.
  // Otherwise return false.
  bool get(const K & k, NodeBase & v) const override {
    return impl.get(k,v);
  }

  void keys(std::set<K> & ks) const override {
    impl.keys(ks);
  };
};

} // namespace wot
