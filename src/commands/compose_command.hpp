#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <nlohmann/json.hpp>

#include <command.hpp>
#include <config.hpp>

COMMAND_START(ComposeCommand)
  COMMAND_CLI("compose")
  COMMAND_SYNOPSIS("wot [--name NAME] [--circle CIRCLE] [...] compose")
  COMMAND_SHORT_DESCRIPTION("Compose a new json node")
  COMMAND_DESCRIPTION(R"(
  Compose a new json node based on the options provided.
  An artifact can be received in stdin and will be edit based on the command
  line options received.
  The node object is then printed to stdout.

  Examples

  # A node not completed
  wot compose --start | wot --name Satoshi compose | wot --circle club compose

  # A completed node, signed and  added to the internal database
  wot compose --start | \
  wot compose --circle circle2 --profile-name name2 --serial 15 | \
  wot compose --circle "c3" | \
  wot compose --unit "EUR" --value 50 --since 3 --to aa --on a --on b | \
  wot compose --profile-key bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq | \
  wot compose --implementation impl | \
  wot sign | wot add
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    std::string in;

    if(vm.count("start")) {
      in = "{}";
    } else {
      if(!Config::get_input(in)) return false;
    }

    nlohmann::json n;
    try{
      n = nlohmann::json::parse(in);
    } catch(nlohmann::json::out_of_range& ex) {
      std::cerr << ex.what() << std::endl;
      return false;
    }

    if(vm.count("circle")) { n["circle"] = vm["circle"].as<std::string>(); }
    if(vm.count("implementation")) {
      n["implementation"] = vm["implementation"].as<std::string>();
    }
    if(vm.count("serial")) { n["serial"] = vm["serial"].as<int>(); }

    for(auto k : {"about", "aka", "dob", "facebook", "key", "name", "nostr",
      "picture", "telegram", "twitter"}) {
      std::string ks("profile-");
      ks += k;
      if(vm.count(ks.c_str())) {
        n["profile"][k] = vm[ks.c_str()].as<std::string>();
      } else {
        if(!n["profile"].contains(k)) {
          n["profile"][k] = "";
        }
      }
    }

    if(vm.count("source")) {
      for(std::string s : vm["source"].as<std::vector<std::string>>()) {
        n["sources"].push_back(s);
      }
    }
    if(!n.contains("sources")) {
      n["sources"] = nlohmann::json::array();
    }

    // Object will be not-signed. If the user want to sign, should be passed
    // to sign
    n["signature"]["hash"] = "";
    n["signature"]["sig"] = "";

    if(vm.count("to") || vm.count("since") || vm.count("unit") || vm.count("value")) {
      if(!vm.count("to") || !vm.count("since") || !vm.count("unit") || !vm.count("value")) {
        std::cerr << "Compose error: to, since, unit and value should all be set together" << std::endl;
        return false;
      }
      nlohmann::json l;
      l["to"] = vm["to"].as<std::string>();
      l["since"] = vm["since"].as<int>();
      l["value"] = vm["value"].as<int>();
      l["unit"] = vm["unit"].as<std::string>();
      if(vm.count("on")) {
        for(std::string on : vm["on"].as<std::vector<std::string>>()) {
          l["on"].push_back(on);
        }
      } else {
        l["on"] = nlohmann::json::array();
      }
      n["trust"].push_back(l);
    }

    std::cout << n.dump() << std::endl;
    return true;
  }
COMMAND_END()
