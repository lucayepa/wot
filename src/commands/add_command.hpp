#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <node.hpp>
#include <db_nodes.hpp>

COMMAND_START(AddCommand)
  COMMAND_CLI("add")
  COMMAND_SHORT_DESCRIPTION("Add an object to the internal db")
  COMMAND_DESCRIPTION(R"(
  Add an object to the internal db.
  The object is read from stdin and is verified before of adding it.

  --force-accept-hash do not check the hash of the object
  --force-accept-sig do not check the signature of the object
)")

  bool act(const boost::program_options::variables_map & vm) const override {
    std::string in;
    if(!Config::get_input(in)) return false;
    Node n(in);
    if( n.verify_node(vm.count("force-accept-hash"),vm.count("force-accept-sig")) ) {
      std::string filename = n.get_signature().get_hash();
      Db_nodes().add_node(filename, in, n.get_json());
      return true;
    } else {
      std::cerr << "Node is not valid" << std::endl;
      return false;
    };
  }
COMMAND_END()
