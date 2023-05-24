#pragma once

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define COMMAND_START(COMMAND_NAME) \
  namespace wot { \
  struct COMMAND_NAME : public Command { \
    std::string get_name() const { return STRINGIFY( COMMAND_NAME ); }

#define COMMAND_END() }; \
  } // namespace wot

#define COMMAND_CLI(x) std::string get_cli() const override { return x; }
#define COMMAND_SYNOPSIS(x) std::string get_synopsis() const override { return x; }
#define COMMAND_SHORT_DESCRIPTION(x) std::string get_short_description() const override { return x; }
#define COMMAND_DESCRIPTION(x) std::string get_description() const override { return x; }

#include <string>

#include <vm_t.hpp>

namespace wot {

// Abstract base class for commands
struct Command {
  virtual ~Command() {}
  virtual std::string get_name() const = 0;
  virtual std::string get_cli() const = 0;
  virtual std::string get_synopsis() const { return(""); };
  virtual std::string get_short_description() const = 0;
  virtual std::string get_description() const = 0;

  // Checks if variables_map is ok for the command. If not, return the error
  // that can be sent to the user
  virtual std::pair<bool, std::string> args_ok(const vm_t & vm) const {
    return {true,""};
  };
  virtual bool act(const vm_t & vm) const = 0;
  virtual bool hidden() const { return false; };
  virtual po::options_description cli_options() const {
    return po::options_description();
  };
};

} // namespace wot
