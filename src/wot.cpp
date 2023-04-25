#include <iostream>
#include <toml.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#include <iomanip>
#include <sstream>
#include <string>
#include <filesystem>

#include <node.hpp>

#include <config.hpp>
#include <disk_db.hpp>
#include <db_nodes.hpp>
#include <cache_sig.hpp>
#include <filters.hpp>

namespace wot {

namespace po = boost::program_options;

} // namespace wot

int main(int argc, char *argv[]) {

  using std::string;

  using namespace wot;

  // ENV vars
  string command = argv[0];
  Config::get().set_command(command);

  const std::string config_help =
    "Alternate config file location (default is HOME_DIR/" + std::string(DEFAULT_CONFIG_FILE) + ")";

  // Declare the supported options.
  string usage = "Usage: "+command+" [-vRTFHI] [--verbose] [--config config_file] [--force-accept-hash] [--force-accept-sig] [--signature] [--rule-filter RULE] [--to-filter TO] "+
  "[from-filter FROM] [hash-filter HASH] [input-file FILE] command [parameter]";
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "help message")
    ("verbose,v", "verbose output")
    ("config", po::value< string >(), config_help.c_str())
    ("force-accept-hash", "Accept object hash, without verification")
    ("force-accept-sig", "Accept signature on object, without verification")
    ("json-output", "Output a TOML object as JSON (signature remains the TOML one) (sign-toml)")
    ("signature", po::value< string >(), "Signature to be added to the local db as a known signatures (add-sig)")
    ("input-file,I", po::value< string >(), "input file")
    ("command", /*po::value< string >()->default_value("verify"),*/ "help | sign | sign-toml | add | verify | ls | ls-rules | view | add-sig | rm-sig | ls-sig")
    ("param", /*po::value< string >(),*/ "parameter (view <hash>, ls <hash>)")
  ;

  for(const auto & f : Filters().all) {
    desc.add_options()
      ( f->get_cli_option().c_str(),
        po::value< string >(),
        f->get_description().c_str()
      );
  }

  po::positional_options_description positional;
  positional.add("command", 1);
  positional.add("param", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(desc).positional(positional).run(), vm);
  if (!vm.count("command")) { vm.emplace("command",po::variable_value((string)"help", true)); }
  po::notify(vm);

  std::map<string,string> help;

  if (!vm.count("verbose")) {
    using namespace boost::log;
    core::get()->set_filter(
      [](const attribute_value_set& attr_set) {
        return attr_set["Severity"].extract<trivial::severity_level>() > trivial::info;
      }
    );
  }

  // Parse the configuration file
  try{
    if(vm.count("config")) {
      Config::get().load(vm["config"].as<string>());
    } else {
      Config::get().load();
    }
  } catch(...) {
    return EXIT_FAILURE;
  };

  help["sign-toml"] = "  " + command + " sign-toml\n" + R"(
  Get a toml object from stdin, add a valid hash and a valid signature.

  --force-accept-hash do not check the hash of the object
  --json-output return a json object (with TOML signature)

  See also `help sign`.
)";
  if (vm["command"].as<string>() == "sign-toml") {
      string s, json, toml;
      try{ get_input(s); } catch(...) {return 1;};
      Node n(s);
      std::optional<std::string> result = n.get_signed(/*as_toml=*/true, vm.count("force-accept-hash"));
      if(result.has_value()) {
        if(vm.count("json-output")) {
          std::cerr << "Json signed object. This object can be verified only if the "
          "TOML source is present. To verify a toml object, see `help verify`."
          << std::endl;
        } else {
          std::cerr << "TOML signed object. To verify a toml object, see "
          "`help verify`." << std::endl;
        }
        std::cout << result.value() << std::endl;
        return 0;
      } else {
        std::cout << "NOT ok" << std::endl;
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
      try{ get_input(s); } catch(...) {return 1;};
      Node n(s);
      auto result = n.get_signed(/*as_toml=*/false, vm.count("force-accept-hash"));
      if(result.has_value()) {
        std::cout << "Signed object:" << std::endl << result.value() << std::endl;
        return 0;
      } else {
        std::cout << "NOT ok" << std::endl;
        return 1;
      }
  }

  help["verify"] = "  " + command + " verify\n" + R"(
  Get an object from stdin and verify if is valid.

  --force-accept-hash do not check the hash of the object
  --force-accept-sig do not check the signature of the object
)";
  if (vm["command"].as<string>() == "verify") {
      string s;
      try{ get_input(s); } catch(...) {return 1;};
      Node n(s);
      bool v = n.verify_node(vm.count("force-accept-hash"),vm.count("force-accept-sig"));
      if(v) {
        std::cout << "ok" << std::endl;
        return 0;
      } else {
        std::cout << "NOT ok" << std::endl;
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
      string in;
      try{ get_input(in); } catch(...) {return 1;};
      Node n(in);
      if( n.verify_node(vm.count("force-accept-hash"),vm.count("force-accept-sig")) ) {
        std::string filename = n.get_signature().get_hash();
        Db_nodes().add_node(filename, in, n.get_json());
        return 0;
      } else {
        std::cerr << "Node is not valid" << std::endl;
        return 1;
      };
  }

  help["ls-rules"] = "  " + command + " ls-rules\n" + R"(
  List all the rules found in the links of the objects of the internal db.
  The number next to every rule is how many times it is present in the links.
)";
  if (vm["command"].as<string>() == "ls-rules") {
      Db_nodes().list_rules();
      return 0;
  }

  help["ls"] = "  " + command + " [--rule-filter RULE] [--to-filter TO] [from-filter FROM] ls\n" + R"(
  List of the objects in the internal db filtered against the provided filters.

  BUG: The objects are first filtered, then are listed as full object.
  This means that if a single link of an object pass the filter, then all the links are shown.
)";
  if (vm["command"].as<string>() == "ls") {
      Db_nodes().list_nodes(vm);
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
      std::cerr << help["add-sig"] << std::endl;
      return 1;
    }
    LOG << "Add-sig: " << vm["param"].as<string>();
    DiskDb("sig").add(vm["param"].as<string>(), vm["signature"].as<string>());
    return 0;
  }

  help["rm-sig"] = "  " + command + " rm-sig <HASH>\n" + R"(
  Remove a trusted signature from the internal db.

  See also `help add-sig`, `help ls-sig`.
)";
  if (vm["command"].as<string>() == "rm-sig") {
    if(!vm.count("param")) {
      std::cerr << help["rm-sig"] << std::endl;
      return 1;
    }
    DiskDb("sig").rm(vm["param"].as<string>());
    return 0;
  }

  help["ls-sig"] = "  " + command + " ls-sig\n" + R"(
  list all the knwon signatures of the internal cache.

  See also `help add-sig`, `help rm-sig`.
)";
  if (vm["command"].as<string>() == "ls-sig") {
    DiskDb("sig").print_list();
    return 0;
  }

  help["view"] = "  " + command + " view <HASH>\n" + R"(
  View the content of the node having hash HASH, from the internal db.
)";
  if (vm["command"].as<string>() == "view") {
    if(!vm.count("param")) {
      std::cerr << help["view"] << std::endl;
      return 1;
    }
    LOG << "View: " << vm["param"].as<string>();
    DiskDb((std::string)"").print(vm["param"].as<string>());
    return 0;
  }

  if (vm["command"].as<string>() == "solve") {
    string s;
    try{ get_input(s); } catch(...) {return 1;};
    toml::table t;
    Node::solve( (std::string_view)(s.c_str()), t );
    std::cout << t << std::endl;
    return 0;
  }

  if (vm.count("help") || vm["command"].as<string>() == "help" ) {
    if(vm.count("param")) {
      //TODO - add a check if the help for that command does not exist
      std::cout << help[vm["param"].as<string>()];
      return 0;
    } else {
      std::cout << usage << std::endl << std::endl;
      std::cout << desc << std::endl;
      std::cout << "help <command> to get help on a command" << std::endl;
      return 0;
    }
  }

  std::cerr << "Not a valid command" << std::endl;
  return 1;
}
