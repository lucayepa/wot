#pragma once

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define FILTER_START(FILTER_NAME) \
  namespace { \
     using namespace wot; \
     struct FILTER_NAME : public wot::Filter { \
    FILTER_NAME() { \
      Config::get().add_filter(this); \
    } \
    std::string get_name() { return STRINGIFY( FILTER_NAME ); }

#define FILTER_END() } _; \
  } // namespace

#define FILTER_DESCRIPTION(x) std::string get_description() override { return x; }
#define FILTER_LONG_DESCRIPTION(x) std::string get_long_description() { return x; }

#include <string>
#include <node.hpp>

namespace wot {

// Base class for node filters
struct Filter {
  virtual ~Filter() {}
  virtual std::string get_name() = 0;
  virtual std::string get_description() = 0;
  virtual std::string get_long_description() = 0;
  virtual bool check(const NodeBase & n, const std::string & arg) const = 0;

  // CamelCase to filter-name-cli-option style
  const std::string get_cli_option();
};

} // namespace wot

// This is not needed by me, but by the filters that includes this file
#include <config.hpp>
