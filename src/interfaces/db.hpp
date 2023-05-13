#pragma once

#include <string>
#include <optional>

namespace wot {

// Key-value db that throws if there are problems and uses simple bool to
// communicate hit and miss.

// A new db object is supposed to find the persistent data by name. No open
// needed.

// This is a low level interface that is supposed to be derived by classes
// implementing this interface on different platforms/media.

// Then we need a high level interface, with filters and print and similar, to
// be derived by real databases, that will contain a low level implementation
// derived by this class.
template<class K, class V>
class Db{
private:
  // Empy string name "" is accepted and is considered the default
  const std::string name;

  // TODO: cache of keys that is loaded the first time that keys() is called
  // and is maintained in add() and rm()

public:
  Db(const std::string & db_name) : name{db_name} {}
  Db() : name{""} {}
  ~Db() {}

  // If key does not exists, add it and return true.
  // Otherwise return false, without adding it.
  virtual bool add(const K &, const V &) = 0;

  // If key exists, remove it and return true.
  // Otherwise return false.
  virtual bool rm(const K &) = 0;

  // If key exists, modify passed V and return true.
  // Otherwise return false.
  virtual bool get(const K &, V &) const = 0;

  // This creates a object of class V. If this is to much overhead, child
  // classes ar supposed to override it.
  virtual bool contains(const K & k) const {
    V _;
    return get(k,_);
  }

  virtual void keys(std::set<K> &) const = 0;
};

} // namespace wot
