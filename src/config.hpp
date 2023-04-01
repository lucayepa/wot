// Read the configuration file
#pragma once
#include <filesystem>

#include <toml.hpp>

#include <interfaces/verifier.hpp>
#include <interfaces/signer.hpp>

namespace wot {

class Config {
private:
  Config() {};

  // This is not in config file. It is the main command name of the program.
  std::string command;

public:
  toml::table config;
  std::shared_ptr<Signer> signer;
  std::shared_ptr<Verifier> verifier;
  
  // TODO: extract it only once at the start
  static std::filesystem::path home_dir();

  static inline std::filesystem::path config_dir() {
    return home_dir() / string(".config/wot");
  };

  static Config & get() {
      static Config instance;
      return instance;
  }
  
  Config(Config const&) = delete;
  void operator=(Config const&)  = delete;
  
  const std::string & get_command() const { return command; }
  void set_command(const std::string & c) { this->command = c; }

  void get_config_from_file();
};

} //namespace wot
