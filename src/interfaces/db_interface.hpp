// Basic db interface. Every object of this class contains a single
// table. The class contains the name of the table. Keys and values
// are strings.

#pragma once

#include <string>
#include <optional>

namespace wot {

class DbInterface {
private:
  // Table "" is accepted and is considered the default
  const std::string table;

public:
  DbInterface(const std::string & table) : table(table) {};
  DbInterface() : table("") {};
  ~DbInterface() {};

  const std::string & get_table() const { return table; }

  virtual bool add(const std::string & key, const std::string & value) = 0;
  virtual bool rm(const std::string & key) = 0;
  virtual std::optional<std::string> get(const std::string & key) const = 0;
  virtual void print(const std::string & key) const = 0;
  virtual void print_list() const = 0;
  // get_filtered();
};

} // namespace wot
