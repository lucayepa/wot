#pragma once

#include <boost/program_options.hpp>

namespace wot {

namespace po = boost::program_options;                                           
typedef po::variables_map vm_t;
std::ostream & operator<<( std::ostream & os, vm_t & vm );
std::string to_string( const vm_t & vm );

} // namespace wot
