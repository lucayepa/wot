#pragma once

#include <string>
#include <node.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

#include <program.hpp>

#include <interfaces/signer.hpp>
#include <interfaces/verifier.hpp>

class ElectrumVerifier : public Verifier {
private:
  Program e{"electrum"};

public:
  ElectrumVerifier() {};
  
  // Return the CLI that can be used to replicate the call to an external program
  virtual string verify_cli(
    const string & signature,
    const string & address,
    const string & message
  );

  virtual bool verify_signature(
    const wot::Node &,
    const string & main_command
  );
};

class ElectrumSigner : public Signer {
private:
  Program e{"electrum"};

public:
  ElectrumSigner() {};
  
  string sign_cli(
    const string & wallet_path,
    const string & address,
    const string & message
  );

  void suggest_external_sign(
    const string & wallet,
    const wot::Node & n,
    const string & main_command
  );

  optional<string> sign(
    const wot::Node & n,
    const string & main_command
  );
};
