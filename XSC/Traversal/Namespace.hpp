// file      : XSC/Traversal/Namespace.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef XSC_TRAVERSAL_NAMESPACE_HPP
#define XSC_TRAVERSAL_NAMESPACE_HPP

#include "XSC/Traversal/Elements.hpp"
#include "XSC/SemanticGraph/Namespace.hpp"

namespace XSC
{
  namespace Traversal
  {
    struct Namespace : ScopeTemplate<SemanticGraph::Namespace>
    {
      virtual void
      traverse (Type& m)
      {
        pre (m);
        name (m);
        names (m);
        post (m);
      }

      virtual void
      pre (Type&)
      {
      }

      virtual void
      name (Type&)
      {
      }

      virtual void
      post (Type&)
      {
      }
    };
  }
}

#endif  // XSC_TRAVERSAL_NAMESPACE_HPP
