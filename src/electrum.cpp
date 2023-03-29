#include <iostream>

#include <electrum.hpp>

using namespace std;
using namespace wot;

inline string ElectrumVerifier::verify_cli(
    const string & signature,
    const string & address,
    const string & message
) {
  return "echo -n " + message + " | " +
    e.get_name() + " --offline verifymessage " + address + " " + signature + " -";
}

bool ElectrumVerifier::verify_signature(const Node & n, const string & main_command){
  e.set_cli( verify_cli( n.get_signature().get_sig(), n.get_profile().get_key(),
    n.get_signature().get_hash() ) );

  if(!e.check_and_suggest_cli("verify", n, main_command)) return false;

  e.set_unique(n.get_signature().get_hash());

  string out;
  e.exec(out);
  return(out == "true\n");
}

// How to sign with electrum. No sanitize. Working only if wallet has no password.
inline string ElectrumSigner::sign_cli(
    const string & wallet_path,
    const string & address,
    const string & message
) {
  return "echo -n " + message + " | " +
      e.get_name() + " --offline -w " + wallet_path + " signmessage " + address + " -";
}

void ElectrumSigner::suggest_external_sign(
    const string & wallet,
    const Node & n,
    const string & main_command
) {

  string sign_cmd = sign_cli(wallet, n.get_profile().get_key(), n.get_signature().get_hash());
  // Command to add a signature to the cache
  string cache_cmd = main_command + " --signature `" + sign_cmd + "` "
    "add-sig " + n.get_signature().get_hash();
  cout << "Please sign the hash using the private key of the address: " <<
    n.get_profile().get_key() << " and add it to the db of known " <<
    "signatures using the following command: " << endl << cache_cmd << endl;
}

optional<string> ElectrumSigner::sign(const Node & n, const string & main_command) {
  const char * env_wallet = getenv("WALLET");

  if(!env_wallet) {
    LOG << "No WALLET in env.";
    cerr << "Environment variable WALLET is not set. If you want to automate"
        "this step, and you have a wallet without password, you can set the"
        "environment variable WALLET to the full path of your wallet." << endl;
    suggest_external_sign( env_wallet?env_wallet:"PATH_TO_YOUR_WALLET", n, main_command );
    return nullopt;
  }

  string scli = sign_cli(env_wallet, n.get_profile().get_key(), n.get_signature().get_hash());
  e.set_cli(scli);
  e.set_unique(n.get_signature().get_hash());

  if(!e.check_and_suggest_cli("sign", n, main_command)) return nullopt;

  string out;
  if(e.exec(out)) {
    LOG << "Command result is not zero";
    return nullopt;
  }

  // Remove dangling \n from result
  out.pop_back();

  return out;
}
