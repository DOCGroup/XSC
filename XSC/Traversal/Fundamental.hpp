// file      : XSC/Traversal/Fundamental.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_TRAVERSAL_FUNDAMENTAL_HPP
#define XSC_TRAVERSAL_FUNDAMENTAL_HPP

#include "XSC/Traversal/Elements.hpp"
#include "XSC/SemanticGraph/Fundamental.hpp"

namespace XSC
{
  namespace Traversal
  {
    using FundamentalType = Node<SemanticGraph::FundamentalType>;
    using AnyType = Node<SemanticGraph::AnyType>;
    using AnySimpleType = Node<SemanticGraph::AnySimpleType>;
    using anyURI = Node<SemanticGraph::AnyUri>;
    using DateTime = Node<SemanticGraph::DateTime>;
    using Byte = Node<SemanticGraph::Byte>;
    using UnsignedByte = Node<SemanticGraph::UnsignedByte>;
    using Short = Node<SemanticGraph::Short>;
    using UnsignedShort = Node<SemanticGraph::UnsignedShort>;
    using Int = Node<SemanticGraph::Int>;
    using UnsignedInt = Node<SemanticGraph::UnsignedInt>;
    using Long = Node<SemanticGraph::Long>;
    using UnsignedLong = Node<SemanticGraph::UnsignedLong>;
    using Decimal = Node<SemanticGraph::Decimal>;
    using Integer = Node<SemanticGraph::Integer>;
    using NonPositiveInteger = Node<SemanticGraph::NonPositiveInteger>;
    using NonNegativeInteger = Node<SemanticGraph::NonNegativeInteger>;
    using PositiveInteger = Node<SemanticGraph::PositiveInteger>;
    using NegativeInteger = Node<SemanticGraph::NegativeInteger>;
    using Boolean = Node<SemanticGraph::Boolean>;
    using Float = Node<SemanticGraph::Float>;
    using Double = Node<SemanticGraph::Double>;
    using String = Node<SemanticGraph::String>;
    using NormalizedString = Node<SemanticGraph::NormalizedString>;
    using Token = Node<SemanticGraph::Token>;
    using Name = Node<SemanticGraph::Name>;
    using NMTOKEN = Node<SemanticGraph::NMTOKEN>;
    using NCName = Node<SemanticGraph::NCName>;
    using QName = Node<SemanticGraph::QName>;
    using Id = Node<SemanticGraph::Id>;
    using IdRef = Node<SemanticGraph::IdRef>;
    using Href = Node<SemanticGraph::Href>;
  }
}


#endif  // XSC_TRAVERSAL_FUNDAMENTAL_HPP
