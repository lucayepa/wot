#pragma once

#include <string>

#include <disk_db.hpp>

namespace wot {

// A set of hashes
template<class T>
class HashSet {
public:
  HashSet() {}
  ~HashSet() {}

  virtual bool contains(const T &) const = 0;

  // If value does not exists, add it and return true.
  // Otherwise return false.
  virtual bool add(const T &) = 0;

  virtual const std::set<T> & get() const = 0;
};

std::ostream & operator<<( std::ostream &, const HashSet<std::string> & );

// A HashSet that is allocated in memory
class MemoryHashSet : public HashSet<std::string> {
private:
  std::set<std::string> s;

public:
  MemoryHashSet() {}
  ~MemoryHashSet() {}

  bool contains(const std::string & h) const override {
    return s.count(h);
  }

  bool add(const std::string & h) override {
    return s.insert(h).second;
  }

  const std::set<std::string> & get() const override {
    return s;
  }
};

// A HashSet that persists.
//
// Needs to be constructed with the name of the db to be used to store the data.
//
// If it is constructed with the name of a DiskDb that already exists, it
// will contain all the keys of the DiskDb. In this case a reset() will erase
// the db.
//
// The reset function is working on the set of keys that were in the database
// at the time of the last get(), so to reset the database, we need to call
// get() followed by reset().
class DiskHashSet : public HashSet<std::string> {
private:
  DiskDb db;
  mutable std::set<std::string> result;
  DiskHashSet() {}
public:
  DiskHashSet(std::string name) : db(name) {}
  ~DiskHashSet() {}

  bool contains(const std::string & h) const override {
    return db.get(h).has_value();
  }

  bool add(const std::string & h) override {
    if(contains(h)) return false;
    if(!db.add(h,"1")) throw(std::runtime_error("File error"));
    return true;
  }

  const std::set<std::string> & get() const override {
    db.keys(result);
    return result;
  }

  void reset() {
    db.keys(result);
    for(auto & h : result) {
      db.rm(h);
    }
  }
};

} // namespace wot
