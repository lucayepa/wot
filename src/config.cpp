#include <config.hpp>

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

#include <electrum.hpp>

namespace wot {

// Not tested on Windows
std::filesystem::path Config::home_dir() {
  char const* home = getenv("HOME");
  if (home or ((home = getenv("USERPROFILE")))) {
    return std::filesystem::path(home);
  } else {
    char const *hdrive = getenv("HOMEDRIVE"),
        *hpath = getenv("HOMEPATH");
    assert(hdrive);  // or other error handling
    assert(hpath);
    return (std::string)std::filesystem::path(hdrive) + (std::string)std::filesystem::path(hpath);
  }
}

void Config::get_config_from_file() {
  try {
    config = toml::parse_file("etc/config.toml");
    LOG << "Loaded config file.";
    if(config["signer"].value<string>().value_or("electrum") == "electrum"){
      signer = std::make_shared<ElectrumSigner>();
    }
    if(config["verifier"].value<string>().value_or("electrum") == "electrum") {
      verifier = std::make_shared<ElectrumVerifier>();
    }
  }
  catch (const toml::parse_error& err) {
    std::cerr << "Parsing failed: " << err << std::endl;
    throw;
  }
}

} // namespace wot
