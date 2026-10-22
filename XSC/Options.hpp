// file      : XSC/Options.hpp

#ifndef XSC_OPTIONS_HPP
#define XSC_OPTIONS_HPP

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace XSC
{
  class Options
  {
  public:
    void
    add (std::string const& name, std::string const& value = {})
    {
      values_[name].push_back (value);
    }

    void
    set (std::string const& name, std::string const& value)
    {
      values_[name] = {value};
    }

    bool
    count (std::string const& name) const noexcept
    {
      return values_.count (name) != 0;
    }

    std::string const&
    value (std::string const& name) const
    {
      auto const i {values_.find (name)};

      if (i == values_.end () || i->second.empty ())
        throw std::out_of_range ("missing command-line option: " + name);

      return i->second.front ();
    }

    std::vector<std::string> const&
    values (std::string const& name) const
    {
      auto const i {values_.find (name)};

      if (i == values_.end ())
        throw std::out_of_range ("missing command-line option: " + name);

      return i->second;
    }

  private:
    std::unordered_map<std::string, std::vector<std::string>> values_;
  };
}

#endif  // XSC_OPTIONS_HPP
