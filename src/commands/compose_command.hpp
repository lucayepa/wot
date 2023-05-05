#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <nlohmann/json.hpp>

#include <command.hpp>
#include <config.hpp>

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

  void fill_with(
    const vm_t & vm,
    nlohmann::json & n
  ) const noexcept {
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

    // Suppose args are already checked, so this means there are since, value
    // and unit as well (see args_ok)
    if(vm.count("to")) {
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
  }

  // Complete a node JSON object with all the required information
  // to be accepted to be a node. They are listed in the `required`
  // fields of the json schema.
  //
  // This is not enough to be fed to the `add` command, because of
  // another constraint like the identity that should be well formed.
  void complete(nlohmann::json & n) const noexcept {
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

  bool act(const vm_t & vm) const override {
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

    fill_with(vm, n);
    complete(n);

    std::cout << n.dump() << std::endl;
    return true;
  }

  std::pair<bool, std::string> args_ok(const vm_t & vm) const override {
    if(
      ( vm.count("to") ||
        vm.count("since") ||
        vm.count("unit") ||
        vm.count("value") )
      &&
      ( !vm.count("to") ||
        !vm.count("since") ||
        !vm.count("unit") ||
        !vm.count("value") )
    ) {
      return {false,
        "Compose error: to, since, unit and value should all be set together"};
    }
    return Command::args_ok(vm);
  }

  po::options_description cli_options() const override {
    po::options_description od("Compose command options (help compose)");
    od.add_options()
      ("start", "start a new node")
      ("implementation", po::value< std::string >(), "implementation")
      ("circle", po::value< std::string >(), "circle")
      ("profile-about", po::value< std::string >(), "about")
      ("profile-aka", po::value< std::string >(), "aka")
      ("profile-dob", po::value< std::string >(), "dob")
      ("profile-facebook", po::value< std::string >(), "facebook")
      ("profile-key", po::value< std::string >(), "key")
      ("profile-name", po::value< std::string >(), "name")
      ("profile-nostr", po::value< std::string >(), "nostr")
      ("profile-picture", po::value< std::string >(), "picture")
      ("profile-telegram", po::value< std::string >(), "telegram")
      ("profile-twitter", po::value< std::string >(), "twitter")
      ("source", po::value< std::vector<std::string> >()->multitoken(), "source to be added to sources array - can be used multiple times")
      ("serial", po::value< int >(), "serial")
      ("to", po::value< std::string >(), "to")
      ("since", po::value< int >(), "since")
      ("unit", po::value< std::string >(), "unit")
      ("value", po::value< int >(), "value")
      ("on", po::value< std::vector<std::string> >()->multitoken(), "on - can be set multiple times")
    ;
    return od;
  };

COMMAND_END()
