// Read the configuration file
#pragma once

#include <toml.hpp>

#include <interfaces/verifier.hpp>
#include <interfaces/signer.hpp>

class Config {
private:
  Config() {};

  // This is not in config file. It is the main command name of the program.
  std::string command;

public:
  toml::table config;
  shared_ptr<Signer> signer;
  shared_ptr<Verifier> verifier;
  
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