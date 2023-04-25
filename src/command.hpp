#pragma once

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define COMMAND_START(COMMAND_NAME) \
  namespace wot { \
  struct COMMAND_NAME : public Command { \
    std::string get_name() { return STRINGIFY( COMMAND_NAME ); }

#define COMMAND_END() }; \
  } // namespace wot

#define COMMAND_CLI(x) std::string get_cli() override { return x; }
#define COMMAND_SYNOPSIS(x) std::string get_synopsis() const override { return x; }
#define COMMAND_SHORT_DESCRIPTION(x) std::string get_short_description() override { return x; }
#define COMMAND_DESCRIPTION(x) std::string get_description() override { return x; }

#include <string>

#include <boost/program_options.hpp>

namespace wot {

// Abstract base class for commands
struct Command {
  virtual ~Command() {}
  virtual std::string get_name() = 0;
  virtual std::string get_cli() = 0;
  virtual std::string get_synopsis() const { return(""); };
  virtual std::string get_short_description() = 0;
  virtual std::string get_description() = 0;
  virtual bool args_ok(const boost::program_options::variables_map & vm) const { return true; };
  virtual bool act(const boost::program_options::variables_map & vm) const = 0;
  virtual bool hidden() const { return false; };
};

} // namespace wot
