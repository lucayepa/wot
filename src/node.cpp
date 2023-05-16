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

std::ostream & operator<<( std::ostream & os, const Link & l) {
  std::string on_s = "";
  if( !l.get_on().empty() ) {
    on_s = "[";
    for(const auto & s : l.get_on()){
      on_s += s + ",";
    }
    on_s.pop_back();
    on_s += "]";
  }
  os << l.get_value() << " " << l.get_unit() << " -> " << l.get_to() << " " <<
  on_s << std::endl;
  return os;
}

std::ostream & operator<<( std::ostream & os, const Profile & p) {
  os << p.get_name() << std::endl;
  return os;
}

  NodeBase::NodeBase(const std::string & jsons){
    try {
      auto j = nlohmann::json::parse(jsons);
      from_json(j, *this);
    }
    catch (...) {
      // Wrong json format, or good format, but wrong for our schema
      throw(std::runtime_error("It is not possible to parse node: "+jsons));
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

std::ostream & operator<<( std::ostream & os, const NodeBase & n) {
  auto pk = n.primary_key();
  os << n.get_signature().get_hash() << " (" << pk << ")\n" <<
  " " << n.get_profile().get_name() << "\n";

  for(const auto & link : n.get_trust()) {
    os << " " << link;
  }
  return os;
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

  void NodeBase::print_node_summary(std::ostream & os, bool with_links) const {
    os << get_signature().get_hash() <<
      " " << get_profile().get_name() <<
      " (" << get_profile().get_key() <<"."<< get_circle() << ")" << std::endl;
    if(not with_links) return;
    for(const auto & link : get_trust()) {
      os << link;
    }
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
    auto hash = s.get_hash();

    if(!sanitize()) throw(std::runtime_error("Signature not possible."));

    // Check if there is already a signature in the cache for this hash
    auto sig_db = DiskDb("sig");
    if(!sig_db.contains(hash)) {
      LOG << "No signature for this hash in the cache.";

      // Try to sign the message
      std::optional<string> sig = Config::get().get_signer()
        ->sign(*this, Config::get().get_command());

      if(!sig.has_value()) throw(std::runtime_error("Signature not possible"));

      // Add the signature to the cache of known signatures
      sig_db.add(hash,sig.value());

      LOG << "Signature written in the cache";
    }

    LOG << "There is a signature for this hash in the cache";
    Signature sig2 = get_signature();
    sig2.set_sig(sig_db.get(hash));
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
         LOG << " ...wrong hash";
         return false;
      }
      LOG << " ...hash is ok";
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
R"(
Input node hash is not a valid json-calculated hash, nor a valid TOML hash.
If you want to use this hash, you can force the program to accept it. In this
case the node will be added to the internal database. The node cannot be
verified though.

If you want to accept the node, please re-run the program, adding the option:
--force-accept-hash.
)" <<
    "   Hash on file: " << get_signature().get_hash() << "\n" <<
    "Calculated hash: " << hash_calc() << "\n\n" <<
    "TO GENERATE A HASH BASED ON A TOML FILE\n" <<
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
