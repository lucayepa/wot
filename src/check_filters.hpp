#pragma once

#include <config.hpp>
#include <filter.hpp>
#include <vm_t.hpp>

namespace wot {

template<class T>
bool check_single_filter(
  const Filter<T>* f,
  const vm_t & vm,
  const T & target,
  const GraphView * context = nullptr
) {
  const std::string name = f->name();
  const std::string cli_option = f->cli_option();
  bool res;
  switch(f->tokens()) {
    case 1: {
      const auto arg = vm[cli_option].as<std::string>();
      res = f->check(target,arg,context);
      break;
    }
    case 0:
      res = f->check(target,context);
      break;
    default: {
      const auto arg_v = & vm[cli_option].as<std::vector<std::string>>();
      res = f->check(target,*arg_v,context);
    }
  }

  if( res ) {
    LOG << name << " passed";
    return true;
  } else {
    LOG << name << " NOT passed";
    return false;
  }
}

template<class T>
bool check_filters(
  const vm_t & vm,
  const T & target,
  const GraphView * context = nullptr
) {
  // All the filters are evaluated using "and" mode
  auto filters = Config::get().get_filters<T>();
  for(const auto & [_, f] : filters) {
    // This function will be called twice, with or without context. We need
    // to use the right set of filters.
    if (context == nullptr && f->needs_context()) continue;
    if (context != nullptr && !f->needs_context()) continue;
    if( !vm.count(f->cli_option()) ) continue;
    if( !check_single_filter(f, vm, target, context) ) return false;
  }
  return true;
}

// This function is slightly different, because it removes from the vector the
// links that do not match the link filters provided
void apply_filters(
  const vm_t & vm,
  std::vector<wot::Link> & trust
) {
  auto it = std::begin(trust);
  for(const auto l : trust){
    if(!wot::check_filters(vm, l)) trust.erase(it);
    it++;
  }
}

} // namespace wot
