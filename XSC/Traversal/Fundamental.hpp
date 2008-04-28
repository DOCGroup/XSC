// file      : XSC/Traversal/Fundamental.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef XSC_TRAVERSAL_FUNDAMENTAL_HPP
#define XSC_TRAVERSAL_FUNDAMENTAL_HPP

#include "XSC/Traversal/Elements.hpp"
#include "XSC/SemanticGraph/Fundamental.hpp"

namespace XSC
{
  namespace Traversal
  {
    typedef
    Node<SemanticGraph::FundamentalType>
    FundamentalType;

    typedef
    Node<SemanticGraph::AnyType>
    AnyType;

    typedef
    Node<SemanticGraph::AnySimpleType>
    AnySimpleType;
    
    typedef
    Node<SemanticGraph::AnyUri>
    anyURI;

    typedef
    Node<SemanticGraph::Byte>
    Byte;

    typedef
    Node<SemanticGraph::UnsignedByte>
    UnsignedByte;

    typedef
    Node<SemanticGraph::Short>
    Short;

    typedef
    Node<SemanticGraph::UnsignedShort>
    UnsignedShort;

    typedef
    Node<SemanticGraph::Int>
    Int;

    typedef
    Node<SemanticGraph::UnsignedInt>
    UnsignedInt;

    typedef
    Node<SemanticGraph::Long>
    Long;

    typedef
    Node<SemanticGraph::UnsignedLong>
    UnsignedLong;

    typedef
    Node<SemanticGraph::Decimal>
    Decimal;

    typedef
    Node<SemanticGraph::Integer>
    Integer;

    typedef
    Node<SemanticGraph::NonPositiveInteger>
    NonPositiveInteger;

    typedef
    Node<SemanticGraph::NonNegativeInteger>
    NonNegativeInteger;

    typedef
    Node<SemanticGraph::PositiveInteger>
    PositiveInteger;

    typedef
    Node<SemanticGraph::NegativeInteger>
    NegativeInteger;

    typedef
    Node<SemanticGraph::Boolean>
    Boolean;

    typedef
    Node<SemanticGraph::Float>
    Float;

    typedef
    Node<SemanticGraph::Double>
    Double;


    typedef
    Node<SemanticGraph::String>
    String;

    typedef
    Node<SemanticGraph::NormalizedString>
    NormalizedString;

    typedef
    Node<SemanticGraph::Token>
    Token;

    typedef
    Node<SemanticGraph::Name>
    Name;

    typedef
    Node<SemanticGraph::NMTOKEN>
    NMTOKEN;

    typedef
    Node<SemanticGraph::NCName>
    NCName;

    typedef
    Node<SemanticGraph::QName>
    QName;

    typedef
    Node<SemanticGraph::Id>
    Id;

    typedef
    Node<SemanticGraph::IdRef>
    IdRef;

    typedef
    Node<SemanticGraph::Href>
    Href;
  }
}


#endif  // XSC_TRAVERSAL_FUNDAMENTAL_HPP
