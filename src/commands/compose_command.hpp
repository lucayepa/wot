#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <nlohmann/json.hpp>

#include <command.hpp>
#include <config.hpp>

namespace {

// Complete a node JSON object with all the required information
// to be accepted to be a node. They are listed in the `required`
// fields of the json schema.
//
// This is not enough to be fed to the `add` command, because of
// another constraint like the identity that should be well formed.
void complete(nlohmann::json & n) {
  for(auto k : {"circle", "implementation"}) {
    if(!n.contains(k)) { n[k] = ""; }
  }

  if(!n.contains("serial")) { n["serial"] = 0; }

  for(auto k : {"profile", "signature"}) {
    if(!n.contains(k)) {
      n[k] = nlohmann::json::object();
    }
  }

  for(auto k : {"hash", "sig"}) {
    if(!n["signature"].contains(k)) { n["signature"][k] = ""; }
  }

  for(auto k : {"sources", "trust"}) {
    if(!n.contains(k)) {
      n[k] = nlohmann::json::array();
    }
  }

  for(auto k : {"about", "aka", "dob", "facebook", "key", "name", "nostr",
    "picture", "telegram", "twitter"}) {
    if(!n["profile"].contains(k)) {
      n["profile"][k] = "";
    }
  }
}

} // namespace

COMMAND_START(ComposeCommand)
  COMMAND_CLI("compose")
  COMMAND_SYNOPSIS("wot [--name NAME] [--circle CIRCLE] [...] compose")
  COMMAND_SHORT_DESCRIPTION("Compose a new node object")
  COMMAND_DESCRIPTION(R"(
  Compose a new node object based on the options provided.
  A JSON artifact can be received from stdin and will be edited.
  A JSON node object is then printed to stdout.

  `--start` says to compose an object without consider stdin, but to generate
  a new node. If there is no `--start` option, then the stdin will be parsed as
  a JSON object.

  If there is an option `--source`, the argument will be added to the sources
  array.

  If there is one of `--unit`, `--value`, `--since`, `--to`, `--on`, all of
  them need to be present. `--on` can be used multiple times. A new trust link
  will be added to the object.

  All of the options --profile-*, if not specified or present in the object
  passed in stdin, will be considered empty strings.

  The following default values will be considered for other options that are
  not specified and are not in the object received from stdin:
  `--circle "" --serial 0 --implementation ""`

  The signature information will not be altered, but usually they are empty,
  because the hash of the node will be altered by this command, so it needs
  to be signed after this command.

  Examples:

  # A simple node
  wot compose --start | \
  wot compose --profile-key bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq

  # A completed node, signed and added to the internal database
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
      }
    }

    if(vm.count("source")) {
      for(std::string s : vm["source"].as<std::vector<std::string>>()) {
        n["sources"].push_back(s);
      }
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

    complete(n);

    std::cout << n.dump() << std::endl;
    return true;
  }
COMMAND_END()
