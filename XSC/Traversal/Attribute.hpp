// file      : XSC/Traversal/Attribute.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_TRAVERSAL_ATTRIBUTE_HPP
#define XSC_TRAVERSAL_ATTRIBUTE_HPP

#include "XSC/SemanticGraph/Attribute.hpp"
#include "XSC/Traversal/Elements.hpp"

namespace XSC
{
  namespace Traversal
  {
    //
    //
    //
    struct Attribute : Node<SemanticGraph::Attribute>
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

#endif  // XSC_TRAVERSAL_ATTRIBUTE_HPP
