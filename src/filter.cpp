#include <filter.hpp>

namespace wot {

// CamelCase to filter-name-cli-option style
const std::string Filter::get_cli_option() {
  const std::string name = get_name();
  std::string out;
  out.push_back(tolower(name[0]));
  std::for_each(name.begin()+1, name.end(), [&] (char const & c) {
    if (isupper(c)) {
      out.push_back('-');
      out.push_back(tolower(c));
    } else {
      out.push_back(c);
    }
  });
  return out;
};

} // namespace wot
