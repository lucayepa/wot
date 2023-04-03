#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <program.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(Program_suite)

BOOST_AUTO_TEST_CASE(get_name) {
  Program p = Program("ls");
  std::string s = p.get_name();
  BOOST_CHECK_EQUAL(s,"ls");
}

BOOST_AUTO_TEST_CASE(set_and_get_cli) {
  Program p = Program("ls");
  std::string cli = "This is a test cli";
  p.set_cli(cli);
  std::string s = p.get_cli();
  BOOST_CHECK_EQUAL(s,cli);
}

BOOST_AUTO_TEST_CASE(check_and_suggest_cli) {
  Program p = Program("ls");
  Node n;
  bool b = p.check_and_suggest_cli("action", n, "main_command");
  BOOST_CHECK(b);
}

BOOST_AUTO_TEST_CASE(check_and_suggest_cli2) {
  Program p = Program("lsssss");
  Node n;
  bool b = p.check_and_suggest_cli("action", n, "main_command");
  BOOST_CHECK(!b);
}

BOOST_AUTO_TEST_CASE(exec) {
  Program p = Program("ls");
  Node n;
  bool b = p.check_and_suggest_cli("action", n, "main_command");
  BOOST_CHECK(b);
  p.set_unique("1234");
  p.set_cli("ls -la");
  std::string out;
  int res = p.exec(out);
  BOOST_CHECK_EQUAL(res,0);
  BOOST_CHECK(boost::algorithm::contains(out,".."));
}

BOOST_AUTO_TEST_SUITE_END()
