#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>

#include <config.hpp>
#include <commands.hpp>

namespace {

template<class T>
void add_filters_to_option_description(
  boost::program_options::options_description & options
) {
  namespace po = boost::program_options;
  auto filters = wot::Config::get().get_filters<T>();
  for(const auto & [k, f] : filters) {
    const auto name = f->cli_option();
    const auto desc = f->desc();
    switch(f->tokens()) {
      case 1:
        options.add_options()
          ( name.c_str(), po::value<std::string>(), desc.c_str() );
        break;
      case 0:
        options.add_options()
          ( name.c_str(), desc.c_str() );
        break;
      default:
        using multi_t = std::vector<std::string>;
        options.add_options()
          ( name.c_str(), po::value<multi_t>()->multitoken(), desc.c_str() );
    }
  }

}

void set_verbose(bool on) {
  using boost::log::core;
  using boost::log::attribute_value_set;
  namespace trivial = boost::log::trivial;
  if(on) {
    auto filter = [](const attribute_value_set& attr_set) {
      return true;
    };
    core::get()->set_filter( filter );
  } else {
    auto filter = [](const attribute_value_set& attr_set) {
      return
        attr_set["Severity"].extract<trivial::severity_level>() > trivial::info;
    };
    core::get()->set_filter( filter );
  }
}

} //namespace

int main(int argc, char *argv[]) {

  namespace po = boost::program_options;

  using namespace wot;

  Config::get().set_argc(argc);
  Config::get().set_argv(argv);

  const std::string config_help =
    "Alternate config file location (default is HOME_DIR/" +
      Config::default_config_file + ")";

  const std::string options_help =
    "Options file. See `help template-options-file`";

  // Declare the supported options.
  std::string usage = "Usage: wot [OPTIONS] command [parameter]";
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "help message")
    ("verbose,v", "verbose output")
    ("config", po::value< std::string >(), config_help.c_str())
    ("options", po::value< std::string >(), options_help.c_str())
    ("force-accept-hash", "Accept node hash, without verification")
    ("force-accept-sig", "Accept signature on node, without verification")
    ("force-no-db-check", "Do not use internal database to verify the object")
    ("json-output", "Output a JSON object (if the command is sign-toml, the "
      "signature remains the TOML one) (sign-toml, view)")
    ("jsonl", "Export the matched nodes as json lines (ls-nodes)")
    ("signature", po::value< std::string >(), "Signature to be added to the "
      "local db as a known signatures (add-sig)")
    ("input-file,I", po::value< std::string >(), "input file")
    ("algo", po::value< std::string >(), "alorithm to use for identities (i.e. "
      "bitcoin)")
    ("signer", po::value< std::string >(), "signer helper (i.e. electrum)")
    ("verifier", po::value< std::string >(), "verifier helper (i.e. electrum)")
    ("command", po::value< std::string >(),
      "Command to execute (can also be the first positional argument")
    ("param", po::value< std::string >(),
      "Argument of the command (can also be the second positional argument)")
  ;

  po::options_description cmdline_options;
  cmdline_options.add(desc);

  po::options_description node_filter_options(
    "Node filters (use with add, ls-nodes or ls)"
  );
  add_filters_to_option_description<NodeBase>(node_filter_options);
  cmdline_options.add(node_filter_options);

  po::options_description link_filter_options(
    "Link filters (use with ls)"
  );
  add_filters_to_option_description<Link>(link_filter_options);
  cmdline_options.add(link_filter_options);

  po::positional_options_description positional;
  positional.add("command", 1);
  positional.add("param", 1);

  std::stringstream commands_help;
  commands_help << "Commands:\n" << std::setw(27) << std::left <<
    "  help" << "This help message\n";

  std::map<std::string,std::string> help;

  for(const auto & c : Commands().all) {
    if(c->cli_options().options().size() > 0) {
      cmdline_options.add(c->cli_options());
    }
    if(!c->hidden()) {
      commands_help <<
        std::setw(27) << std::left << "  " + c->get_cli() <<
        c->get_short_description() << "\n";
      help[c->get_cli()] = c->get_synopsis() + "\n" + c->get_description();
    }
  }

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
    options(cmdline_options).
    positional(positional).run(), vm);

  // Check verbose right now, so we can use LOG in Config.load
  if (!vm.count("verbose")) {
    set_verbose(false);
  }

  // Parse the configuration file
  try{
    bool res;
    if(vm.count("config")) {
      res = Config::get().load(vm["config"].as<std::string>());
    } else {
      res = Config::get().load();
    }
    if(!res) throw(std::runtime_error(""));
  } catch(...) {
    std::cerr << "Error while parsing config file" << std::endl;
    return EXIT_FAILURE;
  };

  if(vm.count("options")) {
    auto filename = vm["options"].as<std::string>();
    po::store(parse_config_file(filename.c_str(), cmdline_options),vm);
  }

  Config::get().set_vm(vm);

  set_verbose(vm.count("verbose"));

  if (!vm.count("command")) {
    vm.emplace("command",po::variable_value((std::string)"help", true));
  }

  for(const auto & c : Commands().all) {
    if (vm["command"].as<std::string>() == c->get_cli()) {
      if(!c->args_ok(vm).first) {
        std::cerr << c->args_ok(vm).second << std::endl;
        return EXIT_FAILURE;
      }
      if(!c->act(vm)) return EXIT_FAILURE;
      return EXIT_SUCCESS;
    }
  }

  if (vm.count("help") || vm["command"].as<std::string>() == "help" ) {
    if(vm.count("param")) {
      std::string param{vm["param"].as<std::string>()};
      if(help.count(param)) {
        std::cout << help[param];
        return EXIT_SUCCESS;
      } else {
        std::cerr << "No help content for command " << param << std::endl;
        return EXIT_FAILURE;
      }
    } else {
      std::cout << usage << std::endl << std::endl;
      std::cout << commands_help.str() << std::endl;
      std::cout << "help <command> to get help on a command\n";
      std::cout << cmdline_options << std::endl;
      return EXIT_SUCCESS;
    }
  }

  std::cerr << "Not a valid command" << std::endl;
  return EXIT_FAILURE;
}
