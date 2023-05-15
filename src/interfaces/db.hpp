#pragma once

#include <string>
#include <optional>

namespace wot {

// Key-value db that throws if there are problems and uses simple bool to
// communicate out of add and rm.

// Get assumes that the control on existence has been made by the caller, so the
// key should always exist.

// A new db object is supposed to find the persistent data by name. No open
// needed.

// This is a low level interface that is supposed to be derived by classes
// implementing this interface on different platforms/media.

// Then we need a high level interface, with filters and print and similar, to
// be derived by real databases, that will contain a low level implementation
// derived by this class.

// V should have move semantics.
template<class K, class V>
class Db{
private:
  // Empy string name "" is accepted and is considered the default
  const std::string name;

  // TODO: cache of keys that is loaded the first time that keys() is called
  // and is maintained in add() and rm()

public:
  // A new created object is supposed to find the persistent data of the
  // underlying database. No open needed.
  Db(std::string db_name) : name{db_name} {}
  Db() : name{""} {}
  ~Db() {}

  std::string get_database_name() const { return name; }

  // If key does not exists, add it and return true.
  // Otherwise return false, without adding it.
  virtual bool add(const K &, const V &) = 0;

  // If key exists, remove it and return true.
  // Otherwise return false.
  virtual bool rm(const K &) = 0;

  // Caller knows that object exists (for example has just called contains)
  virtual V get(const K &) const = 0;

  virtual bool contains(const K & k) const {
    return keys().count(k);
  }

  // Can be faster, but this is only a bridge, because nobody should use this
  virtual void keys(std::set<K> & ks) const {
    for(const K & k : keys()) {ks.insert(k);}
  };
  virtual std::set<K> keys() const = 0;
};

} // namespace wot
