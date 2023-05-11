#include <iostream>

#include <nlohmann/json.hpp>

#include <node.hpp>
#include <math.hpp>
#include <program.hpp>
#include <cache_sig.hpp>
#include <config.hpp>
#include <identity.hpp>
#include <signature.hpp>
#include <filter.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace {

// Can be done in C++, but we mimic the user via CLI
std::string remove_two_lines(const std::string & toml) {
  // This removes comments as well
  // return cli_filter(toml,"grep -v ^\\# | grep . | head -n -2");
  return wot::Program::cli_filter(toml,"head -n -2");
}

} // namespace
namespace wot {

  NodeBase::NodeBase(const std::string & jsons){
    try {
      auto j = nlohmann::json::parse(jsons);
      from_json(j, *this);
    }
    catch (...) {
      // Wrong json format, or good format, but wrong for our schema
      throw(std::runtime_error("It is not possible to parse node"));
    }
  }

  NodeBase::NodeBase(const nlohmann::json & j){
    try {
      LOG << "Is it valid for the schema?";
      from_json(j, *this);
      LOG << "Yes.";
    } catch (nlohmann::json::out_of_range& ex) {
      LOG << "No. Node object does not match our protocol (is it composed "
        "with a different version?)";
      std::cerr << ex.what() << std::endl;
      throw(std::runtime_error("The node is not valid"));
    }
  }

  nlohmann::json Node::parse(const std::string & in) {
    nlohmann::json j;
    try {
      LOG << "Parsing object: " << in;
      LOG << "Is it a valid json? ";
      j = nlohmann::json::parse(in); // throws ej
      LOG << "Yes.";
      origin_is_toml = false;
    } catch (nlohmann::json::parse_error& ej) {
      LOG << "No. Not a valid JSON";
      LOG << "Is it a valid TOML?";
      toml::table t;
      try {
        std::string_view sv = in;
        t = toml::parse(sv); // throws ep
        LOG << "Yes. TOML parsed.";
        LOG << "Is it solvable?";
        solve( sv, t ); // can throw
        LOG << "TOML solved:\n" << t;
      } catch (const toml::parse_error& ep) {
        LOG << "No.";
        std::cerr << "If it was supposed to be JSON, parse error at byte " <<
          ej.byte << std::endl;
        std::cerr << "If it was supposed to be TOML, parse error: " << ep <<
          std::endl;
        throw(std::runtime_error(
          "Object is not a recognized format (JSON or TOML)"));
      } catch (...) {
        throw(std::runtime_error(
          "Object is TOML, but cannot be solved as a valid node object"));
      }
      std::stringstream ss;
      ss << toml::json_formatter{t};
      j = nlohmann::json::parse(ss.str());
      origin_is_toml = true;
    }
    return j;
  }

  // solve a TOML object by removing the two helping hashes: defaults and ref
  void Node::solve( std::string_view in, toml::table & t ) {
    t = toml::parse(in);
    if (toml::array* trust = t["trust"].as_array()) {

      // Substitute defaults
      if (toml::table* defaults = t["defaults"].as_table()) {
        defaults->for_each([&](const toml::key& key, auto&& val){
          trust->for_each([&](auto&& link){link.as_table()->emplace(key,val);});
        });
        t.as_table()->erase("defaults");
      };

      // Substitute ref
      if (t["ref"].as_table()) {
        t["ref"].as_table()
          ->for_each([&](const toml::key& key, auto&& rule_value){
          if (toml::array* trust = t["trust"].as_array()) {
            trust->for_each([&](auto&& link){
              auto a = link.as_table()->at((std::string_view)"on").as_array();
              a->for_each([&](auto&& b, size_t index){
                if(b.value_or((std::string_view)"")==(std::string_view)key) {
                  a->replace(a->cbegin()+index,rule_value);
                }
              });
            });
          };
        });
        t.as_table()->erase("ref");
      };
    };
  }

  void NodeBase::print_node_summary(bool with_links) const {
    std::cout << get_signature().get_hash() << std::endl;
    std::cout << " " << get_profile().get_name() <<
      " (" << get_profile().get_key() <<"."<< get_circle() << ")" << std::endl;
    if(not with_links) return;
    for(const auto & link : get_trust()) {
      print_link_summary(link);
    }
  }

  void NodeBase::print_link_summary(const Link & l) const {
    std::cout << " " << l.get_value() << " " << l.get_unit() <<
      " -> " << l.get_to() << std::endl;
  }

  std::string NodeBase::to_j(const bool withsig) const {
    auto j = nlohmann::json::object();
    j["circle"] = get_circle();
    j["version"] = get_version();
    j["profile"] = get_profile();
    j["serial"] = get_serial();
    j["sources"] = get_sources();
    j["trust"] = get_trust();
    if (withsig) {
      j["signature"] = get_signature();
    }
    return j.dump();
  }

  /*
   In order to do the same from CLI:
   vi /tmp/test.json
   tr --delete '\n' < /tmp/test.json | sha256sum
  */
  std::string Node::hash_calc() const {
    if(origin_is_toml) {
      // Can be done in C++, but we mimic the user via CLI
      // to remove comments we can add this in front:
      // grep -v ^\\# | grep . | ...
      return Program::cli_filter(in,
        "head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\\n'");
    } else {
      std::string j = to_j(/*withsig=*/false);
      LOG << "json string without signature: " << j;
      std::string s = sha256(j);
      LOG << "SHA256 of the json string: " << s;
      return s;
    }
  }

  // Check some fields of the node. Useful before of system calls.
  bool Node::sanitize() const {
    if( get_signature().get_sig()!="signature_here" &&
       !(Signature(*this).is_well_formed()) ) {
      LOG << "Not a well formed signature";
      return false;
    }
    if( !Identity(*this).is_well_formed() ) {
      LOG << "Not a well formed identity key (bitcoin address)";
      return false;
    }
    return true;
  }

  bool Node::signature_verify() const {
    using std::endl;

    if(!sanitize()) return false;

    if(Cache_sig().signature_verify_from_cache(*this)) return true;

    if(Config::get().get_verifier()
         ->verify_signature(*this, Config::get().get_command())) {
      // Add the signature to the cache of known signatures
      DiskDb("sig").add(get_signature().get_hash(),get_signature().get_sig());
      return true;
    } else {
      std::cerr << "Signature is NOT correct\n" <<
      "Hash checked: " << get_signature().get_hash() << "\n" <<
      "     Address: " << get_profile().get_key() << "\n" <<
      "   Signature: " << get_signature().get_sig() << "\n" <<
      "Command to verify: \n" <<
      Config::get().get_verifier()
        ->verify_cli(
            get_signature().get_sig(),
            get_profile().get_key(),
            get_signature().get_hash()) << "\n\n" <<
      "If, on the other side, you want to generate a valid signature: " <<
      Config::get().get_command() << " help sign" << std::endl;
      return false;
    }
  }

  void Node::generate_hash() {
    Signature sig = get_signature();
    sig.set_hash(hash_calc());
    set_signature(sig);
  }

  void Node::sign() {
    // Assume the signature on node is of no importance. Overwrite with a token
    // that can be consumed by sanitize
    Signature s = get_signature();
    s.set_sig("signature_here");
    set_signature(s);

    if(!sanitize()) throw(std::runtime_error("Signature not possible."));

    // Check if there is already a signature in the cache for this hash
    auto sig_on_file = DiskDb("sig").get(s.get_hash());
    if(!sig_on_file.has_value()) {
      LOG << "No signature for this hash in the cache.";

      // Try to sign the message
      std::optional<string> sig = Config::get().get_signer()
        ->sign(*this, Config::get().get_command());

      if(!sig.has_value()) throw(std::runtime_error("Signature not possible"));

      // Add the signature to the cache of known signatures
      DiskDb("sig").add(s.get_hash(),sig.value());

      sig_on_file.emplace(sig.value());
      LOG << "Signature written in the cache";
    }

    LOG << "There is a signature for this hash in the cache";
    Signature sig2 = get_signature();
    sig2.set_sig(sig_on_file.value());
    set_signature(sig2);
  }

  std::string Node::get_signed(bool force_accept_hash) {
    if(!force_accept_hash) { generate_hash(); }

    sign();

    if(origin_is_toml) {
      std::string s = remove_two_lines(in);
      return s +
        "hash = \"" + get_signature().get_hash() + "\"\n" +
        "sig = \"" + get_signature().get_sig() + "\"\n";
    }
    return to_j(/*with_sig=*/true);
  }

  bool Node::verify_node( const vm_t & vm ) const {
    try{
      if( Identity(*this).is_well_formed() ) {
        LOG << get_profile().get_key() << " is a well formed identity key.";
      } else {
        std::cerr << get_profile().get_key() <<
          " is not a well formed identity key.";
        throw;
      };
      if( Signature(*this).is_well_formed() ) {
        LOG << get_signature().get_sig() << " is a well formed signature.";
      } else {
        std::cerr << get_signature().get_sig() <<
          " is not a well formed signature.";
        throw;
      };
    } catch(...) {
      LOG << "Not a good node";
      return false;
    };

    if(vm.count("force-accept-hash")) {
      LOG << "Hash accepted because of force-accept-hash option";
    } else {
      LOG << "Verify hash...";
      if(hash_calc() != get_signature().get_hash()) {
         LOG << " Wrong hash";
         return false;
      }
      LOG << " Hash is ok";
    }

    if(vm.count("force-accept-sig")) {
      LOG << "Signature accepted because of force-accept-sig option";
    } else {
      bool c = signature_verify();
      LOG << "Signature is " << (c ? "" : "NOT ") << "correct.";
      if(!c) return false;
    }

    if(vm.count("force-no-db-check")) {
      LOG << "Primary key accepted because of force-no-db-check option";
      return true;
    }
    auto f = Config::get().get_filters();
    bool d = f["NewSerialFilter"]->check(*this);
    LOG << "Primary key " << get_profile().get_key() << "."
      << get_circle() << "." << get_serial() << " is " <<
      (d ? "" : "NOT ") << "ok.";
    if(d) return true;

    return false;
  }

  bool Node::verify_hash() const {
    if( hash_calc() == get_signature().get_hash() ) return true;

    std::cout <<
    "Input node without a valid json-calculated hash nor a valid TOML hash.\n"<<
    "If you are sure of the hash, you can rerun the program, adding " <<
    "the option `--force-accept-hash`.\n\n" <<
    "   Hash on file: " << get_signature().get_hash() << "\n" <<
    "Calculated hash: " << hash_calc() << "\n\n" <<
    "TO VERIFY A TOML HASH\n" <<
    "Check the requirements in doc/canonic/toml.md\n" <<
    "cat $FILE | head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\\n'" <<
    std::endl;
    return false;
  }

bool NodeBase::check_filters(const vm_t & vm) const {
  // All the filters are evaluated using "and" mode
  auto filters = Config::get().get_filters();
  for(const auto & [k, f] : filters) {
    const std::string cli_option = f->cli_option();
    if( !vm.count(cli_option) ) continue;
    const std::string name = f->name();
    bool res;
    switch(f->tokens()) {
      case 1: {
        const auto arg = vm[cli_option].as<std::string>();
        res = f->check(*this,arg);
        break;
      }
      case 0:
        res = f->check(*this);
        break;
      default: {
        const auto arg_v = & vm[cli_option].as<std::vector<std::string>>();
        res = f->check(*this,*arg_v);
      }
    }
    if( res ) {
      LOG << name << " passed";
      continue;
    } else {
      LOG << name << " NOT passed";
      return false;
    }
  }
  return true;
}

} // namespace wot
