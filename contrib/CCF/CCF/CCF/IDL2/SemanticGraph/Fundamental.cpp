// file      : CCF/IDL2/SemanticGraph/Fundamental.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

// Note, that this file is automatically generated!
//

#include "CCF/IDL2/SemanticGraph/Fundamental.hpp"

namespace CCF
{
  namespace IDL2
  {
    namespace SemanticGraph
    {
      using Introspection::TypeInfo;
      using Introspection::Access;

      // FundamentalType
      //
      //
      namespace
      {
        TypeInfo
        fundamental_type_init_ ()
        {
          TypeInfo ti (typeid (FundamentalType));
          ti.add_base (Access::PUBLIC, true, Type::static_type_info ());
          return ti;
        }

        TypeInfo fundamental_type_ (fundamental_type_init_ ());
      }

      TypeInfo const& FundamentalType::
      static_type_info () { return fundamental_type_; }

      FundamentalType::
      ~FundamentalType ()
      {
      }


      // Object
      //
      //
      namespace
      {
        TypeInfo
        object_init_ ()
        {
          TypeInfo ti (typeid (Object));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo object_ (object_init_ ());
      }

      TypeInfo const& Object::
      static_type_info () { return object_; }


      // ValueBase
      //
      //
      namespace
      {
        TypeInfo
        value_base_init_ ()
        {
          TypeInfo ti (typeid (ValueBase));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo value_base_ (value_base_init_ ());
      }

      TypeInfo const& ValueBase::
      static_type_info () { return value_base_; }


      // Any
      //
      //
      namespace
      {
        TypeInfo
        any_init_ ()
        {
          TypeInfo ti (typeid (Any));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo any_ (any_init_ ());
      }

      TypeInfo const& Any::
      static_type_info () { return any_; }


      // Boolean
      //
      //
      namespace
      {
        TypeInfo
        boolean_init_ ()
        {
          TypeInfo ti (typeid (Boolean));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo boolean_ (boolean_init_ ());
      }

      TypeInfo const& Boolean::
      static_type_info () { return boolean_; }


      // Char
      //
      //
      namespace
      {
        TypeInfo
        char_init_ ()
        {
          TypeInfo ti (typeid (Char));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo char_ (char_init_ ());
      }

      TypeInfo const& Char::
      static_type_info () { return char_; }


      // Double
      //
      //
      namespace
      {
        TypeInfo
        double_init_ ()
        {
          TypeInfo ti (typeid (Double));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo double_ (double_init_ ());
      }

      TypeInfo const& Double::
      static_type_info () { return double_; }


      // Float
      //
      //
      namespace
      {
        TypeInfo
        float_init_ ()
        {
          TypeInfo ti (typeid (Float));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo float_ (float_init_ ());
      }

      TypeInfo const& Float::
      static_type_info () { return float_; }


      // Long
      //
      //
      namespace
      {
        TypeInfo
        long_init_ ()
        {
          TypeInfo ti (typeid (Long));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo long_ (long_init_ ());
      }

      TypeInfo const& Long::
      static_type_info () { return long_; }


      // LongDouble
      //
      //
      namespace
      {
        TypeInfo
        long_double_init_ ()
        {
          TypeInfo ti (typeid (LongDouble));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo long_double_ (long_double_init_ ());
      }

      TypeInfo const& LongDouble::
      static_type_info () { return long_double_; }


      // LongLong
      //
      //
      namespace
      {
        TypeInfo
        long_long_init_ ()
        {
          TypeInfo ti (typeid (LongLong));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo long_long_ (long_long_init_ ());
      }

      TypeInfo const& LongLong::
      static_type_info () { return long_long_; }


      // Octet
      //
      //
      namespace
      {
        TypeInfo
        octet_init_ ()
        {
          TypeInfo ti (typeid (Octet));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo octet_ (octet_init_ ());
      }

      TypeInfo const& Octet::
      static_type_info () { return octet_; }


      // Short
      //
      //
      namespace
      {
        TypeInfo
        short_init_ ()
        {
          TypeInfo ti (typeid (Short));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo short_ (short_init_ ());
      }

      TypeInfo const& Short::
      static_type_info () { return short_; }


      // String
      //
      //
      namespace
      {
        TypeInfo
        string_init_ ()
        {
          TypeInfo ti (typeid (String));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo string_ (string_init_ ());
      }

      TypeInfo const& String::
      static_type_info () { return string_; }


      // UnsignedLong
      //
      //
      namespace
      {
        TypeInfo
        unsigned_long_init_ ()
        {
          TypeInfo ti (typeid (UnsignedLong));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo unsigned_long_ (unsigned_long_init_ ());
      }

      TypeInfo const& UnsignedLong::
      static_type_info () { return unsigned_long_; }


      // UnsignedLongLong
      //
      //
      namespace
      {
        TypeInfo
        unsigned_long_long_init_ ()
        {
          TypeInfo ti (typeid (UnsignedLongLong));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo unsigned_long_long_ (unsigned_long_long_init_ ());
      }

      TypeInfo const& UnsignedLongLong::
      static_type_info () { return unsigned_long_long_; }


      // UnsignedShort
      //
      //
      namespace
      {
        TypeInfo
        unsigned_short_init_ ()
        {
          TypeInfo ti (typeid (UnsignedShort));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo unsigned_short_ (unsigned_short_init_ ());
      }

      TypeInfo const& UnsignedShort::
      static_type_info () { return unsigned_short_; }


      // Void
      //
      //
      namespace
      {
        TypeInfo
        void_init_ ()
        {
          TypeInfo ti (typeid (Void));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo void_ (void_init_ ());
      }

      TypeInfo const& Void::
      static_type_info () { return void_; }


      // Wchar
      //
      //
      namespace
      {
        TypeInfo
        wchar_init_ ()
        {
          TypeInfo ti (typeid (Wchar));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo wchar_ (wchar_init_ ());
      }

      TypeInfo const& Wchar::
      static_type_info () { return wchar_; }


      // Wstring
      //
      //
      namespace
      {
        TypeInfo
        wstring_init_ ()
        {
          TypeInfo ti (typeid (Wstring));
          ti.add_base (
            Access::PUBLIC, true, FundamentalType::static_type_info ());
          return ti;
        }

        TypeInfo wstring_ (wstring_init_ ());
      }

      TypeInfo const& Wstring::
      static_type_info () { return wstring_; }
    }
  }
}
