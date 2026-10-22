// file      : Utility/Introspection/TypeInfo.hpp
// author    : Boris Kolpackov <boris@kolpackov.net>
// copyright : Copyright (c) 2002-2003 Boris Kolpackov
// license   : http://kolpackov.net/license.html

#ifndef UTILITY_INTROSPECTION_TYPE_INFO_HPP
#define UTILITY_INTROSPECTION_TYPE_INFO_HPP

#include <memory>
#include <vector>

#include "Utility/Introspection/TypeId.hpp"

namespace Utility
{
  namespace Introspection
  {
    //
    //
    //
    struct Access
    {
      enum Value
      {
        PRIVATE,
        PROTECTED,
        PUBLIC
      };
    };

    // Forward declaration of class TypeInfo.
    //
    //
    class TypeInfo;

    //
    //
    //
    class BaseInfo
    {
    public:
      BaseInfo (Access::Value access,
                bool virtual_base,
                TypeInfo const& ti);

    public:
      TypeInfo const&
      type_info () const;

      Access::Value
      access () const;

      bool
      virtual_base () const;

    private:
      TypeInfo const* ti_;
      bool virtual_base_;
      Access::Value access_;
    };


    //
    //
    //
    class TypeInfo
    {
    private:
      using BaseInfoList = std::vector<BaseInfo>;

    public:
      using BaseIterator = BaseInfoList::const_iterator;

    public:
      TypeInfo (TypeId const& tid);

      TypeId
      type_id () const;

      BaseIterator
      begin_base () const;

      BaseIterator
      end_base () const;

      void
      add_base (Access::Value access,
                bool virtual_base,
                TypeInfo const& ti);

    private:
      TypeId tid_;
      BaseInfoList base_;
    };
  }
}

#include "Utility/Introspection/TypeInfo.ipp"

#endif  // UTILITY_INTROSPECTION_TYPE_INFO_HPP
