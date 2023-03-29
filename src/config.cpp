#include <config.hpp>

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

#include <electrum.hpp>

void Config::get_config_from_file() {
  try {
    config = toml::parse_file("etc/config.toml");
    LOG << "Loaded config file.";
    if(config["signer"].value<string>().value_or("electrum") == "electrum"){
      signer = make_shared<ElectrumSigner>();
    }
    if(config["verifier"].value<string>().value_or("electrum") == "electrum") {
      verifier = make_shared<ElectrumVerifier>();
    }
  }
  catch (const toml::parse_error& err) {
    std::cerr << "Parsing failed: " << err << endl;
    throw;
  }
}
