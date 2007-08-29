// file      : XSC/SemanticGraph/Fundamental.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

// Note, that this file is automatically generated!
//

#include <XSC/SemanticGraph/Fundamental.hpp>

namespace XSC
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


    // AnyType
    //
    //
    namespace
    {
      TypeInfo
      any_type_init_ ()
      {
        TypeInfo ti (typeid (AnyType));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo any_type_ (any_type_init_ ());
    }

    TypeInfo const& AnyType::
    static_type_info () { return any_type_; }


    // AnySimpleType
    //
    //
    namespace
    {
      TypeInfo
      any_simple_type_init_ ()
      {
        TypeInfo ti (typeid (AnySimpleType));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo any_simple_type_ (any_simple_type_init_ ());
    }

    TypeInfo const& AnySimpleType::
    static_type_info () { return any_simple_type_; }


    // Byte
    //
    //
    namespace
    {
      TypeInfo
      byte_init_ ()
      {
        TypeInfo ti (typeid (Byte));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo byte_ (byte_init_ ());
    }

    TypeInfo const& Byte::
    static_type_info () { return byte_; }


    // UnsignedByte
    //
    //
    namespace
    {
      TypeInfo
      unsigned_byte_init_ ()
      {
        TypeInfo ti (typeid (UnsignedByte));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo unsigned_byte_ (unsigned_byte_init_ ());
    }

    TypeInfo const& UnsignedByte::
    static_type_info () { return unsigned_byte_; }


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


    // Int
    //
    //
    namespace
    {
      TypeInfo
      int_init_ ()
      {
        TypeInfo ti (typeid (Int));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo int_ (int_init_ ());
    }

    TypeInfo const& Int::
    static_type_info () { return int_; }


    // UnsignedInt
    //
    //
    namespace
    {
      TypeInfo
      unsigned_int_init_ ()
      {
        TypeInfo ti (typeid (UnsignedInt));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo unsigned_int_ (unsigned_int_init_ ());
    }

    TypeInfo const& UnsignedInt::
    static_type_info () { return unsigned_int_; }


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


    // Decimal
    //
    //
    namespace
    {
      TypeInfo
      decimal_init_ ()
      {
        TypeInfo ti (typeid (Decimal));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo decimal_ (decimal_init_ ());
    }

    TypeInfo const& Decimal::
    static_type_info () { return decimal_; }


    // Integer
    //
    //
    namespace
    {
      TypeInfo
      integer_init_ ()
      {
        TypeInfo ti (typeid (Integer));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo integer_ (integer_init_ ());
    }

    TypeInfo const& Integer::
    static_type_info () { return integer_; }


    // NonPositiveInteger
    //
    //
    namespace
    {
      TypeInfo
      non_positive_integer_init_ ()
      {
        TypeInfo ti (typeid (NonPositiveInteger));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo non_positive_integer_ (non_positive_integer_init_ ());
    }

    TypeInfo const& NonPositiveInteger::
    static_type_info () { return non_positive_integer_; }


    // NonNegativeInteger
    //
    //
    namespace
    {
      TypeInfo
      non_negative_integer_init_ ()
      {
        TypeInfo ti (typeid (NonNegativeInteger));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo non_negative_integer_ (non_negative_integer_init_ ());
    }

    TypeInfo const& NonNegativeInteger::
    static_type_info () { return non_negative_integer_; }


    // PositiveInteger
    //
    //
    namespace
    {
      TypeInfo
      positive_integer_init_ ()
      {
        TypeInfo ti (typeid (PositiveInteger));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo positive_integer_ (positive_integer_init_ ());
    }

    TypeInfo const& PositiveInteger::
    static_type_info () { return positive_integer_; }


    // NegativeInteger
    //
    //
    namespace
    {
      TypeInfo
      negative_integer_init_ ()
      {
        TypeInfo ti (typeid (NegativeInteger));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo negative_integer_ (negative_integer_init_ ());
    }

    TypeInfo const& NegativeInteger::
    static_type_info () { return negative_integer_; }


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


    // NormalizedString
    //
    //
    namespace
    {
      TypeInfo
      normalized_string_init_ ()
      {
        TypeInfo ti (typeid (NormalizedString));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo normalized_string_ (normalized_string_init_ ());
    }

    TypeInfo const& NormalizedString::
    static_type_info () { return normalized_string_; }


    // Token
    //
    //
    namespace
    {
      TypeInfo
      token_init_ ()
      {
        TypeInfo ti (typeid (Token));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo token_ (token_init_ ());
    }

    TypeInfo const& Token::
    static_type_info () { return token_; }


    // Name
    //
    //
    namespace
    {
      TypeInfo
      name_init_ ()
      {
        TypeInfo ti (typeid (Name));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo name_ (name_init_ ());
    }

    TypeInfo const& Name::
    static_type_info () { return name_; }


    // NMTOKEN
    //
    //
    namespace
    {
      TypeInfo
      NM_TOKEN_init_ ()
      {
        TypeInfo ti (typeid (NMTOKEN));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo NM_TOKEN_ (NM_TOKEN_init_ ());
    }

    TypeInfo const& NMTOKEN::
    static_type_info () { return NM_TOKEN_; }


    // NCName
    //
    //
    namespace
    {
      TypeInfo
      NC_name_init_ ()
      {
        TypeInfo ti (typeid (NCName));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo NC_name_ (NC_name_init_ ());
    }

    TypeInfo const& NCName::
    static_type_info () { return NC_name_; }


    // QName
    //
    //
    namespace
    {
      TypeInfo
      Q_name_init_ ()
      {
        TypeInfo ti (typeid (QName));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo Q_name_ (Q_name_init_ ());
    }

    TypeInfo const& QName::
    static_type_info () { return Q_name_; }


    // Id
    //
    //
    namespace
    {
      TypeInfo
      id_init_ ()
      {
        TypeInfo ti (typeid (Id));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo id_ (id_init_ ());
    }

    TypeInfo const& Id::
    static_type_info () { return id_; }


    // IdRef
    //
    //
    namespace
    {
      TypeInfo
      id_ref_init_ ()
      {
        TypeInfo ti (typeid (IdRef));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo id_ref_ (id_ref_init_ ());
    }

    TypeInfo const& IdRef::
    static_type_info () { return id_ref_; }


    // Href
    //
    //
    namespace
    {
      TypeInfo
      href_init_ ()
      {
        TypeInfo ti (typeid (Href));
        ti.add_base (
          Access::PUBLIC, true, FundamentalType::static_type_info ());
        return ti;
      }

      TypeInfo href_ (href_init_ ());
    }

    TypeInfo const& Href::
    static_type_info () { return href_; }
  }
}
