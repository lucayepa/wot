#include <iostream>
#include <toml.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <filesystem>

#include <node.hpp>

#include <interfaces/verifier.hpp>
#include <interfaces/signer.hpp>
#include <electrum.hpp>

using namespace std;
using namespace wot;
using namespace nlohmann;

namespace po = boost::program_options;

namespace wot {
  inline void to_j(json &j, const Node &x, const bool withsig) {
    j = json::object();
    j["circle"] = x.get_circle();
    j["implementation"] = x.get_implementation();
    j["profile"] = x.get_profile();
    j["serial"] = x.get_serial();
    j["sources"] = x.get_sources();
    j["trust"] = x.get_trust();
    if (withsig) {
      j["signature"] = x.get_signature();
    }
  }
}

namespace global {
  toml::table config;
  shared_ptr<Signer> signer;
  shared_ptr<Verifier> verifier;
  string command;
}

using namespace global;

string sha256(const string str) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, str.c_str(), str.size());
  SHA256_Final(hash, &sha256);
  stringstream ss;
  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << hex << setw(2) << setfill('0') << (int)hash[i];
  }
  return ss.str();
}

/*
 To do the same from CLI:
 vi /tmp/test.json
 tr --delete '\n' < /tmp/test.json | sha256sum
*/
string hash_calc(const Node & t) {
  json j;
  to_j(j, t, /*withsig=*/false);
  LOG << "hash_calc: Object without signature: " << j.dump();
  LOG << "hash_calc: SHA256 of the object: " << sha256(j.dump());
  return sha256(j.dump());
}

// A filter based on CLI (please sanitize before use)
string cli_filter(const string & in, const string & command) {
  ofstream f;
  const string tmp_file1 = "/tmp/in."+sha256(in);
  const string tmp_file2 = "/tmp/out."+sha256(in);
  LOG << "Writing to " << tmp_file1;
  f.open (tmp_file1); f << in; f.close();

  string cli = "cat "+tmp_file1+" | "+command+" > "+tmp_file2;

  LOG << "Command: " << cli;
  system( cli.c_str() );

  ifstream f2 = ifstream(tmp_file2);
  stringstream buffer;
  buffer << f2.rdbuf();
  remove(tmp_file1.c_str());
  remove(tmp_file2.c_str());

  return buffer.str();
}

// Can be done in C++, but we mimic the user via CLI
string hash_calc(const string & toml) {
  // This removes comments as well
  // return cli_filter(toml,"grep -v ^\\# | grep . | head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\\n'");
  return cli_filter(toml,"head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\\n'");
}

// Can be done in C++, but we mimic the user via CLI
string remove_two_lines(const string & toml) {
  // This removes comments as well
  // return cli_filter(toml,"grep -v ^\\# | grep . | head -n -2");
  return cli_filter(toml,"head -n -2");
}

// Add a toml signature to a toml file with hash and sig from node
string add_signature(const string & toml, const Node & n) {
  string s = remove_two_lines(toml);
  return s + "hash = \"" + n.get_signature().get_hash() + "\"\n" + "sig = \"" + n.get_signature().get_sig() + "\"\n";
}

// Not tested on Windows
filesystem::path home_dir() {
  char const* home = getenv("HOME");
  if (home or ((home = getenv("USERPROFILE")))) {
    return filesystem::path(home);
  } else {
    char const *hdrive = getenv("HOMEDRIVE"),
    *hpath = getenv("HOMEPATH");
    assert(hdrive);  // or other error handling
    assert(hpath);
    return (string)filesystem::path(hdrive) + (string)filesystem::path(hpath);
  }
}

inline bool is_bitcoin_address( const string & addr ) {
  regex is_bitcoin( "^(bc1|[13])[a-zA-HJ-NP-Z0-9]{25,62}$" );
  return regex_search( addr, is_bitcoin );
}

inline bool is_base64( const string & sig ) {
  regex base64( "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$" );
  return regex_search( sig, base64 );
}

bool hash_verify(const Node & n) {
  return hash_calc(n) == n.get_signature().get_hash();
}

// Mimic the CLI from a user
bool hash_verify_toml(const string & toml, const Node & n) {
  return hash_calc(toml) == n.get_signature().get_hash();
}

void get_config() {
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

void get_input(string & s) {
  // Get the whole stdin in a string
  s = string(istreambuf_iterator<char>(cin), {});
  LOG << "Loaded input.";
};

filesystem::path db_dir() {
  return home_dir() / string(".local/share/wot");
};

filesystem::path config_dir() {
  return home_dir() / string(".config/wot");
};

stringstream read_file(const string & filename) {
  ifstream f(db_dir()/filename);
  stringstream buffer;
  buffer << f.rdbuf();
  return buffer;
}

void write_file(const string & filename, const string & content) {
  ofstream f;
  LOG << "Writing to " << (string)(db_dir()/filename);
  f.open ((string)(db_dir()/filename));
  f << content;
  f.close();
}

auto remove_file(const string & filename) {
  string s = (string)(db_dir()/filename);
  LOG << "Removing " << s;
  return remove(s.c_str());
}

// add a known signature to the local database
void add_sig(const string & hash, const string & sig) {
  LOG << "Writing signature to " << hash << ".sig as a known signature";
  write_file(hash+".sig",sig);
}

// remove a known signature from the local database
void rm_sig(const string & hash) {
  string filename = hash + ".sig";
  remove_file(filename);
}

// if there is no file at all, return an empty string
string read_sig(const string & hash) {
  stringstream content = read_file(hash+".sig");
  if(content.fail()) return "";
  return content.str();
}

// Check some fields of the node. Useful before of system calls.
bool sanitize(const string & in, const Node & n) {
  if(n.get_signature().get_sig()!="signature_here" && !is_base64(n.get_signature().get_sig())) {
    LOG << "Not a well formed signature";
    return false;
  }
  if(!is_bitcoin_address(n.get_profile().get_key())) {
    LOG << "Not a well formed identity key (bitcoin address)";
    return false;
  }
  if(hash_verify(n) || hash_verify_toml(in,n)) {
    LOG << "Hash is correct";
    return true;
  }
  return false;
}

// Check if we have a good signature on file
// This is useful on systems without electrum
bool signature_verify_from_cache(const Node & n) {
  string sig_on_file = read_sig(n.get_signature().get_hash());
  if(sig_on_file != "") {
    LOG << "We have a signature in our internal db. Check if it is ok.";
    if(sig_on_file == n.get_signature().get_sig()) {
      LOG << "Signature on internal db is correct.";
      return true;
    } else {
      cerr << "Signature on internal db is NOT the same of the object." << endl;
      cerr << "Please verify the signature. In order to remove the signature from "
      "internal db use: " << endl <<
      "rm-sig " << n.get_signature().get_hash() << endl <<
      " or " << endl <<
      "rm -i " << db_dir()/n.get_signature().get_hash() << ".sig" << endl;
      throw;
    }
  }
  return false;
}

bool signature_verify(const string & in,const Node & n) {
  if(!sanitize(in,n)) return false;

  if(signature_verify_from_cache(n)) return true;

  if(verifier->verify_signature(n, command)) {
    // Add the signature to the cache of known signatures
    add_sig(n.get_signature().get_hash(),n.get_signature().get_sig());
    return true;
  } else {
    cerr << "Signature is NOT correct" << endl <<
    "Hash checked: " << n.get_signature().get_hash() << endl <<
    "     Address: " << n.get_profile().get_key() << endl <<
    "   Signature: " << n.get_signature().get_sig() << endl <<
    "Command to verify: " << endl <<
    verifier->verify_cli(n.get_signature().get_sig(),n.get_profile().get_key(),n.get_signature().get_hash()) << endl <<
    endl <<
    "If, on the other side, you want to generate a valid signature: " << command << " help sign" << endl;
    return false;
  }
}

// solve a TOML object by removing the two helping hashes: defaults and rules
void solve( string_view in, toml::table & t ) {
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
            auto a = link.as_table()->at("rules"sv).as_array();
            a->for_each([&](auto&& b, size_t index){
              if(b.value_or(""sv)==(string_view)key) { a->replace(a->cbegin()+index,rule_value); }
            });
          });
        };
      });
      t.as_table()->erase("rules");
    };
  };
}

// Check the object format, and transform toml -> json (if needed)
// Return a valid json in any case and throw if not possible
void jsonize( string & s, bool & is_json ) {
  try {
    LOG << "Is it a valid json? ";
    auto _ = json::parse(s);
    is_json = true;
    LOG << "Yes.";
  } catch (json::parse_error& ej) {
    LOG << "No.";
    LOG << "Is it a valid TOML?";
    toml::table toml;
    try {
      string_view sv = s;
      LOG << "Yes." << s;
      toml = toml::parse(sv);
      LOG << "Yes." << sv;
      is_json = false;
      solve( sv, toml );
      LOG << "Yes." << toml;
    }
    catch (const toml::parse_error& e) {
      LOG << "No." << endl;
      cerr << "If it was supposed to be JSON, parse error at byte " << ej.byte << endl;
      cerr << "If it was supposed to be TOML, parse error: " << e << endl;
      is_json = false;
      throw;
    }
    stringstream ss;
    ss << toml::json_formatter{ toml };
    s = ss.str();
  }
}

void verify_schema( const string & s, Node & n) {
  try {
    LOG << "Is it valid for the schema? ";
    n = json::parse(s);
    LOG << "Yes.";
  }
  catch (json::out_of_range& ex) {
    LOG << "No.";
    cerr << ex.what() << endl;
    throw;
  }
};

bool verify_hash(const bool is_json, const string & in, const Node & n) {
  LOG << "First verify if the hash is based on json";
  bool b = hash_verify(n);
  LOG << "Hash is " << (b ? "" : "NOT ") << "a correct JSON hash.";
  if(b) return b;

  bool c = hash_verify_toml(in,n);
  if(c) return c;

  // TODO: input is json, but verify based on orig file

  cout << "Input is a TOML object, without a valid json-calculated hash nor a valid TOML hash. " << endl <<
  "If you are sure of the hash, you can rerun the program, adding " <<
  "the option `--force-accept-hash`." << endl;
  cout << endl << "   Hash on file: " << n.get_signature().get_hash() << endl;
  cout <<         "JSON based hash: " << hash_calc(n) << endl;
  cout <<         "TOML based hash: " << hash_calc(in) << endl;
  cout << endl << "TO VERIFY A TOML HASH" << endl;
  cout << "Check the requirements in doc/canonic/toml.md" << endl;
  cout << "cat $FILE | head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\\n'" << endl;
  return false;
}

// General node verification
// Modify the node string s
// Return a valid Node structure in n
// Return a valid json in s (having the wrong hash if the input was a toml)
Node empty_node;
bool verify_node( const string & in,
                  string & s,
                  bool force_accept_hash,
                  bool force_accept_sig,
                  Node & n = empty_node ) {
  s = in;
  bool is_json;
  try{
    jsonize(s, is_json);
    // s contains a valid json. Let's see if it is valid against our schema
    verify_schema(s, n);
    if( is_bitcoin_address( n.get_profile().get_key() ) ) {
      LOG << n.get_profile().get_key() << " is a well formed bitcoin address.";
    } else {
      cerr << n.get_profile().get_key() << " is not a well formed bitcoin address.";
      throw;
    };
    if( is_base64( n.get_signature().get_sig() ) ) {
      LOG << n.get_signature().get_sig() << " is a well formed signature.";
    } else {
      cerr << n.get_signature().get_sig() << " is not a well formed signature.";
      throw;
    };
  } catch(...) {
    return false;
  };

  if(force_accept_hash) {
    LOG << "Hash accepted because of force-accept-hash option";
  } else {
    LOG << "Verify hash";
    if(!verify_hash(is_json, in, n)) return false;
  }

  if(force_accept_sig) {
    LOG << "Signature accepted because of force-accept-sig option";
    return true;
  }

  bool c = signature_verify(in,n);
  LOG << "Signature is " << (c ? "" : "NOT ") << "correct.";
  if(c) return true;

  return false;
}


// Generate hash and signature for a node
// if the input is json (is_toml == false), return a valid and signed json node
// else, sign the toml object, and return both a json node with hash and signature that are NOT valid for json,
// but are taken from toml object, and a valid toml object.
//
// Return a valid Node structure in n
// Return a valid json in json (having the wrong hash if the input was a toml)
// return true if signature was correctly generated, false if the user has
// been informed on next steps. In this case json can be half-ready
bool sign_node( const string & in,
                string & json_s,
                bool force_accept_hash,
                bool is_toml,
                Node n,
                string & toml_s ) {

  string solved_in = in;

  if(is_toml) {
    toml::table t;
    solve( (string_view)(in.c_str()), t );
    stringstream ss;
    ss << t;
    solved_in = ss.str();
  }

  // Check the structure of the node (without considering hash and signature)
  // Fill up the json_s string, even if it is not ready with hash and sig
  verify_node(solved_in,json_s,/*force_accept_hash=*/true,/*force_accept_sig=*/true,n);

  string hash;
  if(is_toml) {
    //user wants toml and gave toml
    hash = hash_calc(in);
    LOG << "TOML based hash " << hash << " inserted in the object.";
  } else {
    // Calculate a json hash for the object
    hash = hash_calc(n);
    LOG << "Json based hash " << hash << " inserted in the object.";
  }
  Signature sig = n.get_signature();
  sig.set_hash(hash);
  sig.set_sig("signature_here");
  n.set_signature(sig);

  if(!sanitize(in,n)) return false;

  // Check if there is already a signature in the cache for this hash
  string sig_on_file = read_sig(hash);
  if(sig_on_file == "") {
    LOG << "No signature for this hash in the cache.";

    // Try to sign the message
    optional<string> sig = signer->sign(n, command);
    if(!sig.has_value()) return false;

    // Add the signature to the cache of known signatures
    add_sig(n.get_signature().get_hash(),sig.value());

    sig_on_file = sig.value();
  }

  LOG << "There is a signature for this hash in the cache";
  Signature ss = n.get_signature();
  ss.set_sig(sig_on_file);
  n.set_signature(ss);

  if(is_toml) {
    // set toml for toml requests
    toml_s = add_signature(in,n);
  }
  json j;
  to_j(j, n, /*with_sig=*/true);
  json_s = j.dump();
  return true;
}

// add a node to the local database
void add_node(const string & filename, const string & orig, const string & json) {
  LOG << "Writing " << filename << " having original content: " << orig << " and json content " << json;
  write_file(filename,json);
  if (orig != json) {
    write_file(filename+".orig",orig);
  }
}

// fetch a node from disk, verify it and put it in node object n
void fetch_node(const filesystem::directory_entry & file, Node & n) {
  stringstream content = read_file(file.path().filename());
  LOG << "Checking: " << (string)file.path().filename();
  string json;
  verify_node(content.str(),json,/*force_accpet_hash=*/true,/*force_accpet_sig=*/true,n);
}

// fetch a node from disk, and check if it is ok against given filters
// In order for rule-filter and to-filter to work properly in detailed listing, we should generate an array of nodes reduced by the filters
bool filter_node(const po::variables_map & vm, const filesystem::directory_entry & file) {
  Node n;
  fetch_node(file, n);
  //FILTERS are in 'and'
  bool found = true;
  if(vm.count("to-filter")) {
    found = false;
    string to = vm["to-filter"].as<string>();
    for(const auto & link : n.get_trust()) {
      if(link.get_to() == vm["to-filter"].as<string>()) {
        found = true;
      }
    }
  }
  if(not found) return false;
  if(vm.count("rule-filter")) {
    found = false;
    string rule = vm["rule-filter"].as<string>();
    for(const auto & link : n.get_trust()) {
      auto rules = link.get_rules();
      if(find(rules.begin(), rules.end(), rule) != rules.end()) {
        found = true;
      }
    }
  }
  if(not found) return false;
  if(vm.count("from-filter")) {
    string from = vm["from-filter"].as<string>();
    found = from == n.get_profile().get_key();
  }
  if(not found) return false;
  if(vm.count("hash-filter")) {
    string hash = vm["from-filter"].as<string>();
    found = hash == n.get_signature().get_hash();
  }
  return true;
}

void print_link_summary(const Link & l) {
  cout << " " << l.get_value() << " " << l.get_unit() << " -> " << l.get_to() << endl;
}

void print_node_summary(const Node & n, bool with_links) {
  cout << n.get_signature().get_hash() << endl;
  cout << " " << n.get_profile().get_name() << " (" << n.get_profile().get_key() << ", " << n.get_circle() << ")" << endl;
  if(not with_links) return;
  for(const auto & link : n.get_trust()) {
    print_link_summary(link);
  }
}

// list all the nodes in the database
void list_nodes(const po::variables_map & vm) {
  regex is_toml( "orig$" );
  regex is_sig( "sig$" );
  for (const auto & entry : filesystem::directory_iterator(db_dir())) {
    if(regex_search( (string)entry.path(), is_toml )) continue;
    if(regex_search( (string)entry.path(), is_sig )) continue;
    if(filter_node(vm, entry)) {
      LOG << "Found file " << entry.path().filename();
      stringstream content = read_file(entry.path().filename());
      Node n; string json;
      // Since they are already in our db, do not verify hash and signature
      verify_node(content.str(),json,/*force_accpet_hash=*/true,/*force_accpet_sig=*/true,n);
      print_node_summary(n, /*with_links=*/false);
    }
  }
}

// list all the known signatures in the database
void list_sig() {
  regex is_sig( "sig$" );
  cout << "hash: signature" << endl;
  for (const auto & entry : filesystem::directory_iterator(db_dir())) {
    if(regex_search( (string)entry.path(), is_sig )) {
      stringstream content = read_file(entry.path().filename());
      string filename = entry.path().filename().string();
      cout << filename.substr(0, filename.size()-4) << " : " << content.str() << endl;
    }
  }
}

void list_rules() {
  regex is_toml( "orig$" );
  regex is_sig( "sig$" );
  for (const auto & entry : filesystem::directory_iterator(db_dir())) {
    if(regex_search( (string)entry.path(), is_toml )) continue;
    if(regex_search( (string)entry.path(), is_sig )) continue;
    stringstream content = read_file(entry.path().filename());
    Node n;
    LOG << "Checking: " << (string)entry.path().filename();
    string json;
    verify_node(content.str(),json,/*force_accpet_hash=*/true,/*force_accpet_sig=*/true,n);
    map<string,int> rules;
    for(const auto & link : n.get_trust()) {
      for(const auto & rule : link.get_rules()) {
        rules[rule]++;
      }
    }
    for(const auto & x : rules) {
      cout << x.first << " [" << x.second << "]" << endl;
    }
  }
}

// view a node
void view_node(const string & filename) {
  stringstream content = read_file(filename);
  if(content.fail()) {
    cerr << "File not found!" << endl;
  } else {
    cout << content.str();
  }
}

int main(int argc, char *argv[]) {

  // ENV vars
  command = argv[0];

  // Create database directory
  filesystem::create_directory(db_dir());

  // Declare the supported options.
  string usage = "Usage: "+command+" [-vRTFHI] [--verbose] [--force-accept-hash] [--force-accept-sig] [--signature] [--rule-filter RULE] [--to-filter TO] "+
  "[from-filter FROM] [hash-filter HASH] [input-file FILE] command [parameter]";
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "help message")
    ("verbose,v", "verbose output")
    ("force-accept-hash", "Accept object hash, without verification")
    ("force-accept-sig", "Accept signature on object, without verification")
    ("json-output", "Output a TOML object as JSON (signature remains the TOML one) (sign-toml)")
    ("signature", po::value< string >(), "Signature to be added to the local db as a known signatures (add-sig)")
    ("rule-filter,R", po::value< string >(), "rule filter (ls)")
    ("to-filter,T", po::value< string >(), "to filter (ls)")
    ("from-filter,F", po::value< string >(), "from filter (ls)")
    ("hash-filter,H", po::value< string >(), "hash filter (ls)")
    ("input-file,I", po::value< string >(), "input file")
    ("command", /*po::value< string >()->default_value("verify"),*/ "help | sign | sign-toml | add | verify | ls | ls-rules | view | add-sig | rm-sig | ls-sig")
    ("param", /*po::value< string >(),*/ "parameter (view <hash>, ls <hash>)")
  ;

  po::positional_options_description positional;
  positional.add("command", 1);
  positional.add("param", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(desc).positional(positional).run(), vm);
  if (!vm.count("command")) { vm.emplace("command",po::variable_value("help"s, true)); }
  po::notify(vm);

  map<string,string> help;

  if (!vm.count("verbose")) {
	using namespace boost::log;
    core::get()->set_filter(
      [](const attribute_value_set& attr_set) {
        return attr_set["Severity"].extract<trivial::severity_level>() > trivial::info;
      }
    );
  }

  // Parse the configuration file
  try{ get_config(); } catch(...) {
    return 1;
  };

  help["sign-toml"] = "  " + command + " sign-toml\n" + R"(
  Get a toml object from stdin, add a valid hash and a valid signature.

  --force-accept-hash do not check the hash of the object
  --json-output return a json object (with TOML signature)

  See also `help sign`.
)";
  if (vm["command"].as<string>() == "sign-toml") {
      string s, json, toml;
      Node n;
      try{ get_input(s); } catch(...) {return 1;};
      bool v = sign_node(s, json, vm.count("force-accept-hash"), /*is_toml=*/true, n, toml);
      if(v) {
        if(vm.count("json-output")) {
          cerr << "Json signed object. This object can be verified only if the TOML source is present. To verify a toml object, see `help verify`." << endl;
          cout << json << endl;
        } else {
          cerr << "TOML signed object. To verify a toml object, see `help verify`." << endl;
          cout << toml;
        }
        return 0;
      } else {
        cout << "NOT ok" << endl;
        return 1;
      }
  }

  help["sign"] = "  " + command + " sign\n" + R"(
  Get an object from stdin, add a valid hash and a valid signature.

  --force-accept-hash do not check the hash of the object

  See also `help sign-toml`.
)";
  if (vm["command"].as<string>() == "sign") {
      string s, json, _;
      Node n;
      try{ get_input(s); } catch(...) {return 1;};
      bool v = sign_node(s, json, vm.count("force-accept-hash"), /*is_toml=*/false, n, _);
      if(v) {
        cout << "Signed object:" << endl << json << endl;
        return 0;
      } else {
        cout << "NOT ok" << endl;
        return 1;
      }
  }

  help["verify"] = "  " + command + " verify\n" + R"(
  Get an object from stdin and verify if is valid.

  --force-accept-hash do not check the hash of the object
  --force-accept-sig do not check the signature of the object
)";
  if (vm["command"].as<string>() == "verify") {
      string s, json;
      try{ get_input(s); } catch(...) {return 1;};
      bool v = verify_node(s, json, vm.count("force-accept-hash"),vm.count("force-accept-sig"));
      if(v) {
        cout << "ok" << endl;
        return 0;
      } else {
        cout << "NOT ok" << endl;
        return 1;
      }
  }

  help["add"] = "  " + command + " add\n" + R"(
  Add an object to the internal db."
  The object is readed from stdin and is verified before of adding it."

  --force-accept-hash do not check the hash of the object";
  --force-accept-sig do not check the signature of the object
)";
  if (vm["command"].as<string>() == "add") {
      string in, json;
      try{ get_input(in); } catch(...) {return 1;};
      Node n;
      if( verify_node(in, json, vm.count("force-accept-hash"),vm.count("force-accept-sig"), n) ) {
        string filename = n.get_signature().get_hash();
        add_node(filename, in, json);
        return 0;
      } else {
        cerr << "Node is not valid" << endl;
        return 1;
      };
  }

  help["ls-rules"] = "  " + command + " ls-rules\n" + R"(
  List all the rules found in the links of the objects of the internal db.
  The number next to every rule is how many times it is present in the links.
)";
  if (vm["command"].as<string>() == "ls-rules") {
      list_rules();
      return 0;
  }

  help["ls"] = "  " + command + " [--rule-filter RULE] [--to-filter TO] [from-filter FROM] ls\n" + R"(
  List of the objects in the internal db filtered against the provided filters.

  BUG: The objects are first filtered, then are listed as full object.
  This means that if a single link of an object pass the filter, then all the links are shown.
)";
  if (vm["command"].as<string>() == "ls") {
      list_nodes(vm);
      return 0;
  }

  help["add-sig"] = "  " + command + " --signature <SIGNATURE> add-sig <HASH>\n" + R"(
  Add a trusted signature to the internal db. When a node having hash HASH is found,
  it is considered valid if the signature of the node is the same as SIGNATURE.
  This skips the verification step. It is useful, for example, on systems without an
  installed Electrum executable.

  See also `help ls-sig`, `help rm-sig`.
)";
  if (vm["command"].as<string>() == "add-sig") {
    if(!vm.count("signature") || !vm.count("param")) {
      cerr << help["add-sig"] << endl;
      return 1;
    }
    LOG << "Add-sig: " << vm["param"].as<string>();
    add_sig(vm["param"].as<string>(), vm["signature"].as<string>());
    return 0;
  }

  help["rm-sig"] = "  " + command + " rm-sig <HASH>\n" + R"(
  Remove a trusted signature from the internal db.

  See also `help add-sig`, `help ls-sig`.
)";
  if (vm["command"].as<string>() == "rm-sig") {
    if(!vm.count("param")) {
      cerr << help["rm-sig"] << endl;
      return 1;
    }
    rm_sig(vm["param"].as<string>());
    return 0;
  }

  help["ls-sig"] = "  " + command + " ls-sig\n" + R"(
  list all the knwon signatures of the internal cache.

  See also `help add-sig`, `help rm-sig`.
)";
  if (vm["command"].as<string>() == "ls-sig") {
    list_sig();
    return 0;
  }

  help["view"] = "  " + command + " view <HASH>\n" + R"(
  View the content of the node having hash HASH, from the internal db.
)";
  if (vm["command"].as<string>() == "view") {
    if(!vm.count("param")) {
      cerr << help["view"] << endl;
      return 1;
    }
    LOG << "View: " << vm["param"].as<string>();
    view_node(vm["param"].as<string>());
    return 0;
  }

  if (vm["command"].as<string>() == "solve") {
    string s;
    try{ get_input(s); } catch(...) {return 1;};
    toml::table t;
    solve( (string_view)(s.c_str()), t );
    cout << t << endl;
    return 0;
  }

  if (vm.count("help") || vm["command"].as<string>() == "help" ) {
    if(vm.count("param")) {
      //TODO - add a check if the help for that command does not exist
      cout << help[vm["param"].as<string>()];
      return 0;
    } else {
      cout << usage << endl << endl;
      cout << desc << endl;
      cout << "help <command> to get help on a command" << endl;
      return 0;
    }
  }

  cerr << "Not a valid command" << endl;
  return 1;
}
