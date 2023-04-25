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
#include <commands.hpp>

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
  std::string usage = "Usage: wot [OPTIONS] command [parameter]";
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "help message")
    ("verbose,v", "verbose output")
    ("config", po::value< string >(), config_help.c_str())
    ("force-accept-hash", "Accept node hash, without verification")
    ("force-accept-sig", "Accept signature on node, without verification")
    ("json-output", "Output a TOML node as JSON (signature remains the TOML one) (sign-toml)")
    ("signature", po::value< string >(), "Signature to be added to the local db as a known signatures (add-sig)")
    ("input-file,I", po::value< string >(), "input file")
    ("command", "Command to execute (can even be positional after all the options)")
    ("param", "Argument of the command (can even be positional after command)")
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

  std::stringstream commands_help;
  commands_help << "Commands:\n" << std::setw(27) << std::left <<
    "  help" << "This help message\n";

  for(const auto & c : Commands().all) {
    if(!c->hidden()) {
      commands_help <<
        std::setw(27) << std::left << "  " + c->get_cli() <<
        c->get_short_description() << "\n";
      help[c->get_cli()] = c->get_synopsis() + "\n" + c->get_description();
    }
    if (vm["command"].as<string>() == c->get_cli()) {
      if(!c->args_ok(vm)) {
        std::cerr << help[c->get_cli()] << std::endl;
        return EXIT_FAILURE;
      }
      if(!c->act(vm)) return EXIT_FAILURE;
      return EXIT_SUCCESS;
    }
  }

  if (vm.count("help") || vm["command"].as<string>() == "help" ) {
    if(vm.count("param")) {
      //TODO - add a check if the help for that command does not exist
      std::cout << help[vm["param"].as<string>()];
      return 0;
    } else {
      std::cout << usage << std::endl << std::endl;
      std::cout << commands_help.str() << std::endl;
      std::cout << desc << std::endl;
      std::cout << "help <command> to get help on a command" << std::endl;
      return 0;
    }
  }

  std::cerr << "Not a valid command" << std::endl;
  return 1;
}
