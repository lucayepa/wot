// Basic interface to access a database. Every object of this class contains a
// single table. The class contains the name that identifies the table. Keys
// and values are strings.

#pragma once

#include <string>
#include <optional>

namespace wot {

template<class K, class V>
class DbInterface {
private:
  // Database "" is accepted and is considered the default
  const std::string name;

public:
  // A new created object is supposed to find the persistent data of the
  // underlying database. No open needed.
  DbInterface(const std::string & db) : name{db} {};
  DbInterface() : name("") {};
  ~DbInterface() {};

  const std::string & get_database_name() const { return name; }

  virtual bool add(const K &, const V &) = 0;
  virtual bool rm(const K &) = 0;
  virtual std::optional<V> get(const K &) const = 0;
  virtual void print(const K & key) const = 0;
  virtual void print_list() const = 0;
  // get_filtered();
};

} // namespace wot
