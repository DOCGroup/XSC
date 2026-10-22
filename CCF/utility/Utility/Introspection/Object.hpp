// file      : Utility/Introspection/Object.hpp
// author    : Boris Kolpackov <boris@kolpackov.net>
// copyright : Copyright (c) 2002-2003 Boris Kolpackov
// license   : http://kolpackov.net/license.html

#ifndef UTILITY_INTROSPECTION_OBJECT_HPP
#define UTILITY_INTROSPECTION_OBJECT_HPP

#include <memory>

#include "Utility/Introspection/TypeInfo.hpp"

namespace Utility
{
  namespace Introspection
  {
    class Object
    {
    public:
      static TypeInfo const&
      static_type_info () noexcept;

      virtual TypeInfo const&
      type_info () const noexcept;

    protected:
      virtual
      ~Object ();

      Object () noexcept;
      Object (Object const&) noexcept;

      Object&
      operator= (Object const&) noexcept;

    protected:
      virtual void
      type_info (TypeInfo const& tid) noexcept;

    private:
      TypeInfo const* type_info_;
    };
  }
}

#include "Utility/Introspection/Object.ipp"

#endif  // UTILITY_INTROSPECTION_OBJECT_HPP
