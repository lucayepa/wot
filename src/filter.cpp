#include <filter.hpp>

namespace wot {

// CamelCase to filter-name-cli-option style
template<class T>
std::string Filter<T>::cli_option() const {
  const std::string filter_name = name();
  std::string out;
  out.push_back(tolower(filter_name[0]));
  std::for_each(filter_name.begin()+1, filter_name.end(), [&] (char const & c) {
    if (isupper(c)) {
      out.push_back('-');
      out.push_back(tolower(c));
    } else {
      out.push_back(c);
    }
  });
  return out;
};

template class Filter<NodeBase>;
template class Filter<Link>;

} // namespace wot
