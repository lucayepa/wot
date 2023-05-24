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
  bool insert(const T & e) { return add(e); }

  virtual const std::set<T> get() const = 0;

  bool empty() const { return get().empty(); }
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

  const std::set<std::string> get() const override {
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

  // Since we are writing on the same directory of nodes, we cannot use the
  // default "" database.
  DiskHashSet() {}
public:
  DiskHashSet(std::string name) : db(name) {}
  ~DiskHashSet() {}

  bool contains(const std::string & h) const override {
    return db.contains(h);
  }

  bool add(const std::string & h) override {
    if(contains(h)) return false;
    if(!db.add(h,"1")) throw(std::runtime_error("File error"));
    return true;
  }

  const std::set<std::string> get() const override {
    return db.keys();
  }

  void reset() {
    for(auto & h : db.keys()) {
      db.rm(h);
    }
  }
};

class DiskHashSetWithIndex : public HashSet<std::string> {
private:
  DiskDb db;
  DiskDb index_db;

  // Since we are writing on the same directory of nodes, we cannot use the
  // default "" database.
  DiskHashSetWithIndex() {}
public:
  DiskHashSetWithIndex(std::string name, std::string index_name)
    : db(name), index_db(index_name) {}
  ~DiskHashSetWithIndex() {}

  bool contains(const std::string & h) const override {
    return db.contains(h);
  }
  bool index_contains(const std::string & k) const {
    return index_db.contains(k);
  }

  bool add(const std::string & h) override {
    throw(std::logic_error("Index needed"));
  }

  bool add(const std::string & k, const std::string & h) {
    if(contains(h)) return false;
    if(index_db.contains(k)) {
      std::string old_hash;
      index_db.get(k,old_hash);
      if(!db.rm(old_hash)) throw(std::runtime_error("File error"));
    }
    if(!db.add(h,"1")) throw(std::runtime_error("File error"));
    if(!index_db.add(k,h)) throw(std::runtime_error("File error"));
    return true;
  }

  const std::set<std::string> get() const override {
    return db.keys();
  }
  // Existence of key in index should be checked before of calling this
  const std::string get_by_index(std::string k) const {
    return index_db.get(k);
  }

  void reset() {
    for(auto & h : db.keys()) {
      db.rm(h);
    }
    for(auto & k : index_db.keys()) {
      index_db.rm(k);
    }
  }
};

} // namespace wot
