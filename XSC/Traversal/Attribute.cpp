// file      : XSC/Traversal/Attribute.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "XSC/Traversal/Attribute.hpp"

namespace XSC
{
  namespace Traversal
  {
    // Attribute
    //
    //
    void Attribute::
    traverse (Type& a)
    {
      pre (a);
      belongs (a);
      name (a);
      post (a);
    }

    void Attribute::
    pre (Type&)
    {
    }

    void Attribute::
    belongs (Type& a, EdgeDispatcherBase& d)
    {
      d.dispatch (a.belongs ());
    }

    void Attribute::
    belongs (Type& a)
    {
      belongs (a, edge_traverser ());
    }

    void Attribute::
    name (Type&)
    {
    }

    void Attribute::
    post (Type&)
    {
    }
  }
}
