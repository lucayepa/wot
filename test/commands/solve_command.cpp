#include <boost/test/unit_test.hpp>

#include <commands/solve_command.hpp>
#include <disk_db.hpp>
#include <string>

using namespace wot;

BOOST_AUTO_TEST_SUITE(SolveCommandTest)

//cp -a doc/examples/example.toml test/generated/node1.toml
//build/wot solve < test/generated/node1.toml > test/generated/node1s.toml
//diff test/node1s.toml test/generated/node1s.toml

BOOST_AUTO_TEST_CASE(check) {
  std::string before;
  std::string after;

  // File to be solved
  BOOST_CHECK(DiskDb::generic_read_file("../../../doc/examples/example.toml",before));

  // File solved
  BOOST_CHECK(DiskDb::generic_read_file("../node1s.toml",after));

  std::string result;
  SolveCommand().solve_no_ui(before, result);

  BOOST_CHECK(result == after);
}

BOOST_AUTO_TEST_SUITE_END()
