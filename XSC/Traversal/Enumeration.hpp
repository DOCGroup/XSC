// file      : XSC/Traversal/Enumeration.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_TRAVERSAL_ENUMERATION_HPP
#define XSC_TRAVERSAL_ENUMERATION_HPP

#include "XSC/SemanticGraph/Enumeration.hpp"
#include "XSC/Traversal/Elements.hpp"

namespace XSC
{
  namespace Traversal
  {
    struct Enumeration : ScopeTemplate<SemanticGraph::Enumeration>
    {
      virtual void
      traverse (Type&);

      virtual void
      pre (Type&);

      virtual void
      name (Type&);

      virtual void
      post (Type&);
    };

    struct Enumerator : Node<SemanticGraph::Enumerator>
    {
      virtual void
      traverse (Type&);

      virtual void
      pre (Type&);

      virtual void
      belongs (Type&, EdgeDispatcherBase&);

      virtual void
      belongs (Type&);

      virtual void
      name (Type&);

      virtual void
      post (Type&);
    };
  }
}

#endif  // XSC_TRAVERSAL_ENUMERATION_HPP
