#pragma once

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define FILTER_START(FILTER_NAME) \
  namespace { \
     using namespace wot; \
     struct FILTER_NAME : public wot::Filter<NodeBase> { \
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
template <class T = NodeBase>
struct Filter {
  virtual ~Filter() {}
  virtual std::string name() const = 0;
  virtual std::string desc() const = 0;
  virtual std::string long_desc() const = 0;

  virtual int tokens() const { return 1; }

  // One of these functions is called based on the number of tokens returned by
  // the function tokens(). The same number is used by boost program options to
  // accept the arguments on the command line.
  //
  // If the filter does not override the right function, then it is considered
  // a logic error. So these functions throw a default error.
  virtual bool check(const T & n) const {
    return wrong_args();
  };
  virtual bool check(const T & n, const std::string & arg) const {
    return wrong_args();
  };
  virtual bool check(
    const T & n,
    const std::vector<std::string> & arg
  ) const {
    return wrong_args();
  };

  // When the caller is using the CLI, boost options should check and this
  // will not be used. In case of library call, it is a logic error.
  bool wrong_args() const {
    throw( std::logic_error("Wrong args: " + cli_option()) );
    return false;
  };

  // CamelCase to filter-name-cli-option style
  std::string cli_option() const;
};

} // namespace wot

// This is not needed by me, but by the filters that includes this file
#include <config.hpp>
