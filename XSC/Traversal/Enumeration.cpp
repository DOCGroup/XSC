// file      : XSC/Traversal/Enumeration.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "XSC/Traversal/Enumeration.hpp"

namespace XSC
{
  namespace Traversal
  {
    // Enumeration
    //
    //
    void Enumeration::
    traverse (Type& s)
    {
      pre (s);
      name (s);
      names (s);
      post (s);
    }

    void Enumeration::
    pre (Type&)
    {
    }

    void Enumeration::
    name (Type&)
    {
    }

    void Enumeration::
    post (Type&)
    {
    }


    // Enumerator
    //
    //
    void Enumerator::
    traverse (Type& a)
    {
      pre (a);
      belongs (a);
      name (a);
      post (a);
    }

    void Enumerator::
    pre (Type&)
    {
    }

    void Enumerator::
    belongs (Type& a, EdgeDispatcherBase& d)
    {
      d.dispatch (a.belongs ());
    }

    void Enumerator::
    belongs (Type& a)
    {
      belongs (a, edge_traverser ());
    }

    void Enumerator::
    name (Type&)
    {
    }

    void Enumerator::
    post (Type&)
    {
    }
  }
}
