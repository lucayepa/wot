// Read the configuration file
#pragma once
#include <filesystem>

#include <boost/program_options.hpp>

#include <toml.hpp>

#include <interfaces/verifier.hpp>
#include <interfaces/signer.hpp>
#include <filter.hpp>

namespace{
  // Default config directory RELATIVE TO HOME directory
  constexpr auto DEFAULT_DIR = ".config/wot";
  constexpr auto DEFAULT_FILE = "config.toml";
} // namespace

namespace wot {

namespace {

  std::string default_content =
R"(# Web of trust configuration file

# Possible algoritms for public key and signature accepted
#"nostr" will be here
#"dummy" is used for tests and consider the key always well formed
algos = [ "bitcoin" ]

signer = "electrum"

verifier = "electrum"
)";

} // namespace

class Config {
private:
  // Singleton
  Config() = default;
  ~Config() = default;

  // Absolute path of the config file in use
  std::filesystem::path abs_file;

  // This is not in config file. It is the main command name of the program.
  std::string command;

  std::map<std::string, Filter*> filters;

  // Store the vm here, so anyone can see it through this singleton
  boost::program_options::variables_map vm;

  std::shared_ptr<Signer> signer;
  std::shared_ptr<Verifier> verifier;
  std::vector<std::string> algos;

  // Cache the information that init has been successfully completed
  bool init_done = false;

public:
  // Singleton
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

  static Config & get() {
    static Config instance;
    return instance;
  }

  toml::table config;

  void add_filter(Filter * f) { filters[f->name()] = f; }
  const Filter * get_filter(std::string name) { return filters[name]; }
  std::map<std::string, Filter*> & get_filters() { return filters; }

  const std::string & get_command() const { return command; }
  void set_command(const std::string & c) { this->command = c; }
  std::shared_ptr<Signer> get_signer();
  std::shared_ptr<Verifier> get_verifier();
  const std::vector<std::string> & get_algos() { return algos; };

  const boost::program_options::variables_map & get_vm() const { return vm; }
  void set_vm(const boost::program_options::variables_map & v) { vm = v; }

  inline static std::string default_config_file = (std::string)DEFAULT_DIR+"/"+DEFAULT_FILE;

  const std::filesystem::path & get_abs_file() const { return abs_file; };

  // Get the whole stdin in a string
  // Return false if there is an exception
  // TODO: should load the file in '-I' if there is one
  static bool get_input(std::string & s);

  // Try to load the config. If the file is not there and is different
  // from the default, return error.
  // If the file is not there and is the default file, create it with the
  // default content.
  // file is relative to home dir
  bool load(const std::string & file = std::string());

  // This is used in tests only
  bool forced_load(const std::string & file = std::string()) {
    init_done = false;
    return load(file);
  }
};

} //namespace wot
