#pragma once

#include <config.hpp>

namespace wot {

template<class T>
bool check_filters(const vm_t & vm, const T & target) {
  // All the filters are evaluated using "and" mode
  auto filters = Config::get().get_filters<T>();
  for(const auto & [k, f] : filters) {
    const std::string cli_option = f->cli_option();
    if( !vm.count(cli_option) ) continue;
    const std::string name = f->name();
    bool res;
    switch(f->tokens()) {
      case 1: {
        const auto arg = vm[cli_option].as<std::string>();
        res = f->check(target,arg);
        break;
      }
      case 0:
        res = f->check(target);
        break;
      default: {
        const auto arg_v = & vm[cli_option].as<std::vector<std::string>>();
        res = f->check(target,*arg_v);
      }
    }
    if( res ) {
      LOG << name << " passed";
      continue;
    } else {
      LOG << name << " NOT passed";
      return false;
    }
  }
  return true;
}

} // namespace wot
