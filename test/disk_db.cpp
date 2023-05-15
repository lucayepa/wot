#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <disk_db.hpp>

using namespace wot;

BOOST_AUTO_TEST_SUITE(DiskDbTest)

BOOST_AUTO_TEST_CASE(generic_read_file) {
  std::string s;
  BOOST_CHECK(DiskDb::generic_read_file("/etc/passwd",s));
  BOOST_CHECK(boost::algorithm::contains(s, "root"));

  BOOST_TEST_MESSAGE("This is false because current dir is build dir/test");
  BOOST_CHECK( ! DiskDb::generic_read_file("test/node1s.toml",s));

  BOOST_TEST_MESSAGE("The file is in current directory");
  BOOST_CHECK(DiskDb::generic_read_file("node1s.toml",s));

  BOOST_CHECK(boost::algorithm::contains(s, "circle"));
}

BOOST_AUTO_TEST_CASE(HighLevelInterface) {
  std::string key("KEY");
  std::string value("VALUE");
  auto ddb = DiskDb("test");

  ddb.add(key, value);

  BOOST_CHECK(ddb.contains(key));

  BOOST_CHECK(ddb.get(key) == "VALUE");

  BOOST_CHECK(ddb.rm(key));

  BOOST_CHECK(!ddb.contains(key));
}

BOOST_AUTO_TEST_CASE(ReadonlyDbInterface) {
  // Add a object in test db
  std::string key("KEY");
  std::string value("VALUE");
  DiskDb("test").add(key, value);

  // Scan keys of default db
  std::set<std::string> ks;
  DiskDb("").keys(ks);

  for(const auto & k : ks ) {
    // Test element should not be present in default db
    BOOST_CHECK(k != "KEY.test");
  }

  // Rm object from test db
  BOOST_CHECK(DiskDb("test").rm(key));
}

BOOST_AUTO_TEST_SUITE_END()
