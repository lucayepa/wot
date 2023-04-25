#include <cache_sig.hpp>

#include <fstream>
#include <regex>
#include <optional>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

  // Check if we have a good signature on file
  // This is useful on systems without electrum
  bool Cache_sig::signature_verify_from_cache(const Node n) {
    auto sig_on_file = DiskDb("sig").get(n.get_signature().get_hash());
    if(sig_on_file.has_value()) {
      LOG << "We have a signature in our internal db. Check if it is ok.";
      if(sig_on_file.value() == n.get_signature().get_sig()) {
        LOG << "Signature on internal db is correct.";
        return true;
      } else {
        using std::endl, std::cerr;
        cerr << "Signature on internal db is NOT the same of the object." << endl;
        cerr << "Please verify the signature. In order to remove the signature from "
        "internal db use: " << endl <<
        "rm-sig " << n.get_signature().get_hash() << endl <<
        " or " << endl <<
        "rm -i " << DiskDb().get_dir()/n.get_signature().get_hash() << ".sig" << endl;
        throw;
      }
    }
    return false;
  }

} // namespace wot