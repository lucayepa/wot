#include <cache_sig.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

namespace wot {

// Check if we have a good signature on file
// This is useful on systems without electrum
bool Cache_sig::signature_verify_from_cache(const Node n) {
  auto ddb = DiskDb("sig");
  auto h = n.get_signature().get_hash();
  if(ddb.contains(h)) {
    LOG << "We have a signature in our internal db. Check if it is ok.";
    if(ddb.get(h) == n.get_signature().get_sig()) {
      LOG << "Signature on internal db is correct.";
      return true;
    } else {
      std::cerr << "Signature on internal db is NOT the same of the object.\n"
      << "Please verify the signature. In order to remove the signature from "
      "internal db use:\n" <<
      "rm-sig " << n.get_signature().get_hash() << "\n" <<
      " or " << "\n" <<
      "rm -i " << ddb.get_dir()/n.get_signature().get_hash() << ".sig" <<
      std::endl;
      throw(std::runtime_error("Wrong signature on file"));
    }
  } else {
    return false;
  }
}

} // namespace wot
