#pragma once

#include <string>
#include <memory>

#include <config.hpp>
#include <identity.hpp>
#include <signature.hpp>

namespace {

bool is_bitcoin_address( const std::string & addr ) {
  std::regex is_bitcoin( "^(bc1|[13])[a-zA-HJ-NP-Z0-9]{25,62}$" );
  return std::regex_search( addr, is_bitcoin );
}

bool is_base64( const std::string & sig ) {
  std::regex base64(
    "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$"
  );
  return std::regex_search( sig, base64 );
}

} // namespace

namespace wot {

class Algo {
public:
  Algo() {};
  ~Algo() {};

  virtual std::string name() const = 0;

  virtual bool is_well_formed(const Identity & i) const { return true; }

  virtual bool is_well_formed(const Signature & s) const { return true; }

  virtual bool is_revoked(const Identity & i) const { return false; }

  virtual std::optional<std::string> sign(
    const Node & n,
    const std::string & hash
  ) const { return std::nullopt; }

  virtual bool verify(
    const Node &,
    const std::string & signature
  ) const { return true; }
};

class BitcoinAlgo : public Algo {
public:
  BitcoinAlgo() {};
  ~BitcoinAlgo() {};
  std::string name() const { return "bitcoin"; }

  bool is_well_formed(const Identity & i) const override {
    return is_bitcoin_address(i.get());
  }

  bool is_well_formed(const Signature & s) const override {
    return is_base64(s.get_sig());
  }
};

class DummyAlgo : public Algo {
public:
  DummyAlgo() {};
  ~DummyAlgo() {};
  std::string name() const { return "dummy"; }
};

struct Algos {
  std::map<std::string,std::shared_ptr<Algo>> all;
  std::vector<std::shared_ptr<Algo>> in_use;

  Algos() {
    all[BitcoinAlgo().name()]=std::make_shared<BitcoinAlgo>();
    all[DummyAlgo().name()]=std::make_shared<DummyAlgo>();

    for(auto name : Config::get().get_algos()) {
      in_use.push_back(all[name]);
    }
  }
};

} // namespace wot
