// file      : CCF/CodeGenerationKit/Regex.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef CCF_RUNTIME_REGEX_HPP
#define CCF_RUNTIME_REGEX_HPP

#include <regex>
#include <string>

namespace regex
{
  template <typename C>
  std::basic_string<C>
  perl_s (std::basic_string<C> const& src, std::basic_string<C> const& e)
  {
    using string = std::basic_string<C>;
    using size = typename string::size_type;

    if (e.empty ())
      return src;

    C const delimiter {e[0]};

    size const first {e.find (delimiter)};
    size const middle {e.find (delimiter, first + 1)};
    size const last {e.find (delimiter, middle + 1)};

    string const pattern {e, first + 1, middle - first - 1};
    string const format {e, middle + 1, last - middle - 1};

    std::basic_regex<C> const expr {pattern};

    return std::regex_replace (src, expr, format);
  }

  template <typename C>
  std::basic_string<C>
  perl_s (std::basic_string<C> const& src, C const* e)
  {
    return perl_s (src, std::basic_string<C> (e));
  }
}

#endif  // CCF_RUNTIME_REGEX_HPP
