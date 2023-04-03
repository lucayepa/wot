#include <config.hpp>

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

#include <electrum.hpp>
#include <disk_db.hpp>

namespace wot {

bool Config::load(const std::string & file) {
  if (init_done) return true;

  std::filesystem::path abs_file;

  if(file == std::string()) {
    std::filesystem::path abs_dir = DiskDb().home_dir() / (std::filesystem::path)DEFAULT_DIR;
    abs_file = DiskDb().home_dir() / DEFAULT_CONFIG_FILE;

    DiskDb::generic_check_or_write_file_with_interaction(abs_dir, abs_file, default_content);
  } else {
    abs_file = file;
    if(!DiskDb::generic_file_exists(file)) {
      std::cerr << "Requested config file " << file << " does not exist" << std::endl;
      return false;
    }
  }

  try {
    config = toml::parse_file((std::string)abs_file);
    LOG << "Loaded config file.";
    if(config["signer"].value<string>().value_or("electrum") == "electrum"){
      signer = std::make_shared<ElectrumSigner>();
    }
    if(config["verifier"].value<string>().value_or("electrum") == "electrum") {
      verifier = std::make_shared<ElectrumVerifier>();
    }
    init_done = true;
    return true;
  }
  catch (const toml::parse_error& err) {
    std::cerr << "Parsing failed: " << err << std::endl;
    return false;
  }
}

} // namespace wot
