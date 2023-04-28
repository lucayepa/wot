// Read the configuration file
#pragma once
#include <filesystem>

#include <toml.hpp>

#include <interfaces/verifier.hpp>
#include <interfaces/signer.hpp>

// Default config directory RELATIVE TO HOME directory
#define DEFAULT_DIR ".config/wot"
#define DEFAULT_FILE "config.toml"
#define DEFAULT_CONFIG_FILE DEFAULT_DIR "/" DEFAULT_FILE

namespace wot {

namespace {

  static std::string default_content = R"(
# Web of trust configuration file

#algo = "nostr"
algo = "bitcoin"

signer = "electrum"

verifier = "electrum"
)";

} // namespace

class Config {
private:
  // Singleton
  Config() = default;
  ~Config() = default;

  std::filesystem::path abs_file;

  // This is not in config file. It is the main command name of the program.
  std::string command;

  std::shared_ptr<Signer> signer;
  std::shared_ptr<Verifier> verifier;

  // Cache the information that init has been successfully completed
  bool init_done = false;

public:
  // Singleton
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

  // First call of `get` initialize based on file. Then no file is needed.
  static Config & get() {
    default_config_file = DEFAULT_CONFIG_FILE;
    static Config instance;
    return instance;
  }

  toml::table config;

  const std::string & get_command() const { return command; }
  void set_command(const std::string & c) { this->command = c; }
  const std::shared_ptr<Signer> & get_signer();
  void set_signer(const std::shared_ptr<Signer> & s) { this->signer = s; }
  const std::shared_ptr<Verifier> & get_verifier();
  void set_verifier(const std::shared_ptr<Verifier> & s) { this->verifier = s; }

  inline static std::string default_config_file;

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
};

} //namespace wot
