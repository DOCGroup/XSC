// file      : Utility/ExH/Compound.tpp
// author    : Boris Kolpackov <boris@kolpackov.net>
// copyright : Copyright (c) 2002-2003 Boris Kolpackov
// license   : http://kolpackov.net/license.html

#include "Utility/ExH/Converter.hpp"

namespace Utility
{
  namespace ExH
  {
    // c-tor's & d-tor
    template <typename Type, typename Base>
    Compound<Type, Base>::
    Compound () noexcept
    {
    }

    template <typename Type, typename Base>
    Compound<Type, Base>::
    Compound (char const* description) noexcept
    {
      Base::init (description);
    }

    template <typename Type, typename Base>
    Compound<Type, Base>::
    Compound (std::string const& description) noexcept
    {
      try
      {
        Base::init (description.c_str ());
      }
      catch (...)
      {
      }
    }

    template <typename Type, typename Base>
    template <typename T>
    Compound<Type, Base>::
    Compound (T const& description) noexcept
    {
      Base::init (converter<T> (description).c_str ());
    }

    template <typename Type, typename Base>
    Compound<Type, Base>::
    Compound (Compound const& src) noexcept
          : Base::Base::Base (),
            Base::Base (),
            Base ()
    {
      Base::init (src.what ());
    }

    template <typename Type, typename Base>
    Compound<Type, Base>::
    ~Compound () noexcept
    {
    }

    // operator=

    template <typename Type, typename Base>
    Compound<Type, Base>& Compound<Type, Base>::
    operator= (Compound const& src) noexcept
    {
      Base::init (src.what ());
      return *this;
    }
  }
}
