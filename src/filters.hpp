#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include "filter.hpp"
#include "filters/hash_filter.hpp"
#include "filters/always_true_filter.hpp"
#include "filters/from_filter.hpp"
#include "filters/to_filter.hpp"
#include "filters/rule_filter.hpp"
#include "filters/grep_filter.hpp"
#include "filters/bitcoin_key_filter.hpp"
#include "filters/serial_epoch_in_future_filter.hpp"
#include "filters/serial_epoch_in_past_filter.hpp"
#include "filters/serial_yyyymmdd_filter.hpp"

namespace wot {

// Container of filters
struct Filters {
  std::vector<std::unique_ptr<Filter>> all;

  Filters() {
    all.push_back(std::make_unique<AlwaysTrueFilter>());
    all.push_back(std::make_unique<HashFilter>());
    all.push_back(std::make_unique<FromFilter>());
    all.push_back(std::make_unique<ToFilter>());
    all.push_back(std::make_unique<RuleFilter>());
    all.push_back(std::make_unique<GrepFilter>());
    all.push_back(std::make_unique<BitcoinKeyFilter>());
    all.push_back(std::make_unique<SerialEpochInPastFilter>());
    all.push_back(std::make_unique<SerialEpochInFutureFilter>());
    all.push_back(std::make_unique<SerialYyyymmddFilter>());
  }
};

} // namespace wot
