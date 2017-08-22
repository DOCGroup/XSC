// file      : XSC/Traversal/Element.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_TRAVERSAL_ELEMENT_HPP
#define XSC_TRAVERSAL_ELEMENT_HPP

#include "XSC/Traversal/Elements.hpp"
#include "XSC/SemanticGraph/Element.hpp"

namespace XSC
{
  namespace Traversal
  {
    struct Element : Node<SemanticGraph::Element>
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

#endif  // XSC_TRAVERSAL_ELEMENT_HPP
