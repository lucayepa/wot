#include <iostream>

#include <nlohmann/json.hpp>

#include <node.hpp>
#include <math.hpp>
#include <program.hpp>
#include <cache_sig.hpp>
#include <config.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

  // Can be done in C++, but we mimic the user via CLI
  std::string remove_two_lines(const std::string & toml) {
    // This removes comments as well
    // return cli_filter(toml,"grep -v ^\\# | grep . | head -n -2");
    return Program::cli_filter(toml,"head -n -2");
  }

  // solve a TOML object by removing the two helping hashes: defaults and rules
  void Node::solve( std::string_view in, toml::table & t ) {
    t = toml::parse(in);
    if (toml::array* trust = t["trust"].as_array()) {

      // Substitute defaults
      if (toml::table* defaults = t["defaults"].as_table()) {
        defaults->for_each([&](const toml::key& key, auto&& val){
          trust->for_each([&](auto&& link){ link.as_table()->emplace(key,val); });
        });
        t.as_table()->erase("defaults");
      };

      // Substitute rules
      if (t["rules"].as_table()) {
        t["rules"].as_table()->for_each([&](const toml::key& key, auto&& rule_value){
          if (toml::array* trust = t["trust"].as_array()) {
            trust->for_each([&](auto&& link){
              auto a = link.as_table()->at((std::string_view)"rules").as_array();
              a->for_each([&](auto&& b, size_t index){
                if(b.value_or((std::string_view)"")==(std::string_view)key) { a->replace(a->cbegin()+index,rule_value); }
              });
            });
          };
        });
        t.as_table()->erase("rules");
      };
    };
  }

  // Add a toml signature to a toml file with hash and sig from node
  std::string add_signature(const std::string & toml, const Node & n) {
    std::string s = remove_two_lines(toml);
    return s + "hash = \"" + n.get_signature().get_hash() + "\"\n" + "sig = \"" + n.get_signature().get_sig() + "\"\n";
  }

  void Node::print_node_summary(bool with_links) const {
    using std::cout, std::endl;
    cout << get_signature().get_hash() << endl;
    cout << " " << get_profile().get_name() << " (" << get_profile().get_key() << ", " << get_circle() << ")" << endl;
    if(not with_links) return;
    for(const auto & link : get_trust()) {
      print_link_summary(link);
    }
  }

  bool Node::input_hash_verify_toml() const {
    return hash_calc(in) == get_signature().get_hash();
  }

  bool Node::node_hash_verify() const {
    return hash_calc() == get_signature().get_hash();
  }

  void Node::import(const wot_qt::Node & n) {
    set_circle(n.get_circle());
    set_implementation(n.get_implementation());
    set_profile(n.get_profile());
    set_serial(n.get_serial());
    set_sources(n.get_sources());
    set_trust(n.get_trust());
    set_signature(n.get_signature());
  }

  bool Node::generate_node() {
    try {
      LOG << "Is it valid for the schema? ";
      Node n = nlohmann::json::parse(json_s).get<Node>();
      import(n);
      LOG << "Yes.";
      return true;
    }
    catch (nlohmann::json::out_of_range& ex) {
      LOG << "No.";
      std::cerr << ex.what() << std::endl;
      return false;
    }
  }

  void Node::print_link_summary(const Link & l) const {
    std::cout << " " << l.get_value() << " " << l.get_unit() << " -> " << l.get_to() << std::endl;
  }

  const std::string Node::to_j(const bool withsig) const {
    auto j = nlohmann::json::object();
    j["circle"] = get_circle();
    j["implementation"] = get_implementation();
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
  const std::string Node::hash_calc() const {
    std::string j = to_j(/*withsig=*/false);
    LOG << "hash_calc: Object without signature: " << j;
    std::string s = sha256(j);
    LOG << "hash_calc: SHA256 of the object: " << s;
    return s;
  }

  // Can be done in C++, but we mimic the user via CLI
  std::string Node::hash_calc(const std::string & toml) {
    // This removes comments as well
    // return cli_filter(toml,"grep -v ^\\# | grep . | head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\\n'");
    return Program::cli_filter(toml,"head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\\n'");
  }

  // Check some fields of the node. Useful before of system calls.
  bool Node::sanitize() const {
    if(get_signature().get_sig()!="signature_here" && !Node::is_base64(get_signature().get_sig())) {
      LOG << "Not a well formed signature";
      return false;
    }
    if(!Node::is_bitcoin_address(get_profile().get_key())) {
      LOG << "Not a well formed identity key (bitcoin address)";
      return false;
    }
    if(node_hash_verify() || input_hash_verify_toml()) {
      LOG << "Hash is correct";
      return true;
    }
    return false;
  }

  bool Node::signature_verify() const {
    using std::endl;
    
    if(!sanitize()) return false;

    if(Cache_sig().signature_verify_from_cache(*this)) return true;

    if(Config::get().verifier->verify_signature(*this, Config::get().get_command())) {
      // Add the signature to the cache of known signatures
      DiskDb("sig").add(get_signature().get_hash(),get_signature().get_sig());
      return true;
    } else {
      using std::cerr, std::endl;
      cerr << "Signature is NOT correct" << endl <<
      "Hash checked: " << get_signature().get_hash() << endl <<
      "     Address: " << get_profile().get_key() << endl <<
      "   Signature: " << get_signature().get_sig() << endl <<
      "Command to verify: " << endl <<
      Config::get().verifier->verify_cli(get_signature().get_sig(),get_profile().get_key(),get_signature().get_hash()) << endl <<
      endl <<
      "If, on the other side, you want to generate a valid signature: " << Config::get().get_command() << " help sign" << endl;
      return false;
    }
  }

  // Check the object format, and transform toml -> json (if needed)
  // Populate json_s in any case and return false if not possible
  bool Node::pre_parse() {
    try {
      LOG << "Is it a valid json? ";
      auto _ = nlohmann::json::parse(in);
      LOG << "Yes.";
      json_s = in;
      return true;
    } catch (nlohmann::json::parse_error& ej) {
      LOG << "No.";
      LOG << "Is it a valid TOML?";
      toml::table toml;
      try {
        std::string_view sv = in;
        toml = toml::parse(sv);
        LOG << "Yes: TOML parsed" << std::endl << sv;
        solve( sv, toml );
        LOG << "Yes: TOML solved" << toml;
      }
      catch (const toml::parse_error& e) {
        LOG << "No." << std::endl;
        std::cerr << "If it was supposed to be JSON, parse error at byte " << ej.byte << std::endl;
        std::cerr << "If it was supposed to be TOML, parse error: " << e << std::endl;
        return false;
      }
      std::stringstream ss;
      ss << toml::json_formatter{ toml };
      json_s = ss.str();
      return true;
    }
  }

  std::optional<std::string> Node::get_signed(const bool as_toml, const bool force_accept_hash) {
    string solved_in = in;

    if(as_toml) {
      toml::table t;
      solve( (std::string_view)(in.c_str()), t );
      std::stringstream ss;
      ss << t;
      solved_in = ss.str();
    }

    // Check the structure of the node (without considering hash and signature)
    // Fill up the json_s string, even if it is not ready with hash and sig
    Node n2(solved_in);
    n2.verify_node(/*force_accept_hash=*/true,/*force_accept_sig=*/true);
    json_s = n2.get_json();
    import(n2);

    string hash;
    if(as_toml) {
      // user wants toml and gave toml
      // This is signed in the original form, not in the solved one
      hash = hash_calc(in);
      LOG << "TOML based hash " << hash << " inserted in the object.";
    } else {
      // Calculate a json hash for the object
      // Since it is the json form, it is solved
      hash = hash_calc();
      LOG << "Json based hash " << hash << " inserted in the object.";
    }
    Signature sig = get_signature();
    sig.set_hash(hash);
    sig.set_sig("signature_here");
    set_signature(sig);

    if(!sanitize()) return std::nullopt;

    // Check if there is already a signature in the cache for this hash
    auto sig_on_file = DiskDb("sig").get(hash);
    if(!sig_on_file.has_value()) {
      LOG << "No signature for this hash in the cache.";

      // Try to sign the message
      std::optional<string> sig = Config::get().signer->sign(*this, Config::get().get_command());
      if(!sig.has_value()) return std::nullopt;

      // Add the signature to the cache of known signatures
      DiskDb("sig").add(get_signature().get_hash(),sig.value());

      sig_on_file.emplace(sig.value());
    }

    LOG << "There is a signature for this hash in the cache";
    Signature ss = get_signature();
    ss.set_sig(sig_on_file.value());
    set_signature(ss);

    string toml_s;
    if(as_toml) {
      // set toml for toml requests
      toml_s = add_signature(in,*this);
    }
    json_s = to_j(/*with_sig=*/true);
    return as_toml ? toml_s : json_s;
  }

  bool Node::verify_node( const bool force_accept_hash, const bool force_accept_sig ) {
    try{
      if(!pre_parse()) throw;

      // Let's see if `in` is valid against our schema and populate n
      if(!generate_node()) throw;

      if( Node::is_bitcoin_address( get_profile().get_key() ) ) {
        LOG << get_profile().get_key() << " is a well formed bitcoin address.";
      } else {
        std::cerr << get_profile().get_key() << " is not a well formed bitcoin address.";
        throw;
      };
      if( Node::is_base64( get_signature().get_sig() ) ) {
        LOG << get_signature().get_sig() << " is a well formed signature.";
      } else {
        std::cerr << get_signature().get_sig() << " is not a well formed signature.";
        throw;
      };
    } catch(...) {
      return false;
    };

    if(force_accept_hash) {
      LOG << "Hash accepted because of force-accept-hash option";
    } else {
      LOG << "Verify hash";
      if(!verify_hash()) return false;
    }

    if(force_accept_sig) {
      LOG << "Signature accepted because of force-accept-sig option";
      return true;
    }

    bool c = signature_verify();
    LOG << "Signature is " << (c ? "" : "NOT ") << "correct.";
    if(c) return true;

    return false;
  }

  bool Node::verify_hash() {
    using std::cout, std::endl;
    LOG << "First verify if the hash is based on json";
    bool b = node_hash_verify();
    LOG << "Hash is " << (b ? "" : "NOT ") << "a correct JSON hash.";
    if(b) return b;

    bool c = input_hash_verify_toml();
    if(c) return c;

    // TODO: input is json, but verify based on orig file

    cout << "Input is a TOML object, without a valid json-calculated hash nor a valid TOML hash. " << endl <<
    "If you are sure of the hash, you can rerun the program, adding " <<
    "the option `--force-accept-hash`." << endl;
    cout << endl << "   Hash on file: " << get_signature().get_hash() << endl;
    cout <<         "JSON based hash: " << hash_calc() << endl;
    cout <<         "TOML based hash: " << hash_calc(in) << endl;
    cout << endl << "TO VERIFY A TOML HASH" << endl;
    cout << "Check the requirements in doc/canonic/toml.md" << endl;
    cout << "cat $FILE | head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\\n'" << endl;
    return false;
  }

} // namespace wot
