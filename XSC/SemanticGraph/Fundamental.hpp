// file      : XSC/SemanticGraph/Fundamental.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

// Note, that this file is automatically generated!
//

#ifndef XSC_SEMANTIC_GRAPH_FUNDAMENTAL_HPP
#define XSC_SEMANTIC_GRAPH_FUNDAMENTAL_HPP

#include <XSC/SemanticGraph/Elements.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    //
    //
    //
    class FundamentalType : public virtual Type
    {
    public:
        static Introspection::TypeInfo const&
        static_type_info ();

    protected:
      FundamentalType ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class AnyType : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      AnyType ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class AnySimpleType : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      AnySimpleType ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Byte : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Byte ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class UnsignedByte : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      UnsignedByte ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Short : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Short ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class UnsignedShort : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      UnsignedShort ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Int : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Int ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class UnsignedInt : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      UnsignedInt ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Long : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Long ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class UnsignedLong : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      UnsignedLong ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Decimal : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Decimal ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Integer : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Integer ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class NonPositiveInteger : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      NonPositiveInteger ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class NonNegativeInteger : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      NonNegativeInteger ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class PositiveInteger : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      PositiveInteger ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class NegativeInteger : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      NegativeInteger ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Boolean : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Boolean ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Float : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Float ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Double : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Double ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class String : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      String ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class NormalizedString : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      NormalizedString ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Token : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Token ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Name : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Name ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class NMTOKEN : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      NMTOKEN ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class NCName : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      NCName ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class QName : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      QName ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Id : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Id ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class IdRef : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      IdRef ()
      {
        type_info (static_type_info ());
      }
    };


    //
    //
    //
    class Href : public virtual FundamentalType
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Href ()
      {
        type_info (static_type_info ());
      }
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_FUNDAMENTAL_HPP
