// file      : XSC/Traversal/Complex.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_TRAVERSAL_COMPLEX_HPP
#define XSC_TRAVERSAL_COMPLEX_HPP

#include "XSC/Traversal/Elements.hpp"
#include "XSC/SemanticGraph/Complex.hpp"

namespace XSC
{
  namespace Traversal
  {
    struct Complex : ScopeTemplate<SemanticGraph::Complex>
    {
      virtual void
      traverse (Type&);

      virtual void
      pre (Type&);

      virtual void
      name (Type&);

      virtual bool
      inherits (Type&, EdgeDispatcherBase& d);

      virtual void
      inherits (Type&);

      virtual void
      inherits_pre (Type&);

      virtual void
      inherits_post (Type&);

      virtual void
      inherits_none (Type&);

      virtual void
      post (Type&);

      // Generate
      // read/write operations for
      // reading and writing complex
      // types
      virtual void
      read_write_type (Type&);

      // Generate >> and << operators
      // for reading and writing
      // complex types
      virtual void
      insertion_extraction (Type&);
    };
  }
}

#endif  // XSC_TRAVERSAL_COMPLEX_HPP
