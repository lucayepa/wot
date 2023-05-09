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
    std::string name() const override { return STRINGIFY( FILTER_NAME ); }

#define FILTER_END() } _; \
  } // namespace

#define FILTER_DESC(x) std::string desc() const override { \
  return x; \
}
#define FILTER_LONG_DESC(x) std::string long_desc() const override { \
  return x; \
}
#define FILTER_TOKENS(x) int tokens() const override { \
  return x; \
}

#include <string>
#include <node.hpp>

namespace wot {

// Base class for node filters
struct Filter {
  virtual ~Filter() {}
  virtual std::string name() const = 0;
  virtual std::string desc() const = 0;
  virtual std::string long_desc() const = 0;

  virtual int tokens() const { return 1; }

  virtual bool check(const NodeBase & n) const {
    return wrong_args();
  };
  virtual bool check(const NodeBase & n, const std::string & arg) const {
    return wrong_args();
  };
  virtual bool check(
    const NodeBase & n,
    const std::vector<std::string> & arg
  ) const {
    return wrong_args();
  };

  bool wrong_args() const {
    throw( std::runtime_error("Wrong args: " + cli_option()) );
    return false;
  };

  // CamelCase to filter-name-cli-option style
  std::string cli_option() const;
};

} // namespace wot

// This is not needed by me, but by the filters that includes this file
#include <config.hpp>
