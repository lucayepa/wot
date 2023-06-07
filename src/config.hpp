#pragma once
#include <filesystem>

#include <toml.hpp>

#include <interfaces/verifier.hpp>
#include <interfaces/signer.hpp>
#include <filter.hpp>
#include <vm_t.hpp>
#include <identity.hpp>

namespace{
  // Default config directory RELATIVE TO HOME directory
  constexpr auto DEFAULT_DIR = ".config/wot";
  constexpr auto DEFAULT_FILE = "config.toml";

  template<class T>
  using FilterMap = std::map< std::string, wot::Filter<T>* >;
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
typedef boost::program_options::variables_map vm_t;

private:
  // Singleton
  Config() = default;
  ~Config() = default;

  // Absolute path of the config file in use
  std::filesystem::path abs_file;

  int argc;
  char** argv;

  FilterMap<NodeBase> node_filters;
  FilterMap<Link> link_filters;
  FilterMap<Identity> identity_filters;

  // Store the vm here, so anyone can see it through this singleton
  vm_t vm;

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

  template<class T = NodeBase> void add_filter(Filter<T> * f);
  template<class T = NodeBase> const Filter<T> * get_filter(std::string name);
  template<class T = NodeBase> FilterMap<T> & get_filters();

  po::options_description get_filters_description() const;

  std::string get_command() const {
    if(argv == nullptr) return "wot";
    std::string s = argv[0];
    return s;
  }

  void set_argc(int a) { this->argc = a; }
  void set_argv(char* a[]) { this->argv = a; }
  int get_argc() const { return argc; }
  char** get_argv() const { return argv; }

  std::shared_ptr<Signer> get_signer();
  std::shared_ptr<Verifier> get_verifier();
  const std::vector<std::string> & get_algos() { return algos; };

  const vm_t & get_vm() const { return vm; }
  void set_vm(const vm_t & v) { vm = v; }

  inline static std::string default_config_file =
    (std::string)DEFAULT_DIR + "/" + DEFAULT_FILE;

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
