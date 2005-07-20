// file      : XSC/Traversal/Element.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "XSC/Traversal/Element.hpp"

namespace XSC
{
  namespace Traversal
  {
    void Element::
    traverse (Type& m)
    {
      pre (m);
      belongs (m);
      name (m);
      post (m);
    }

    void Element::
    pre (Type&)
    {
    }

    void Element::
    belongs (Type& m, EdgeDispatcherBase& d)
    {
      d.dispatch (m.belongs ());
    }

    void Element::
    belongs (Type& m)
    {
      belongs (m, edge_traverser ());
    }

    void Element::
    name (Type&)
    {
    }

    void Element::
    post (Type&)
    {
    }
  }
}
