#pragma once

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define FILTER_START(FILTER_NAME) \
  namespace wot { \
  struct FILTER_NAME : public Filter { \
    std::string get_name() { return STRINGIFY( FILTER_NAME ); }

#define FILTER_END() }; \
  } // namespace wot

#define FILTER_DESCRIPTION(x) std::string get_description() override { return x; }
#define FILTER_LONG_DESCRIPTION(x) std::string get_long_description() { return x; }

#include <string>
#include <node.hpp>

namespace wot {

// Abstract base class for node filters
struct Filter {
  virtual ~Filter() {}
  virtual std::string get_name() = 0;
  virtual std::string get_description() = 0;
  virtual bool check(const Node & n, const std::string & arg) const = 0;

  // CamelCase to filter-name-cli-option style
  const std::string get_cli_option();
};

} // namespace wot
