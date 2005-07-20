// file      : XSC/Traversal/Elements.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "XSC/Traversal/Elements.hpp"

namespace XSC
{
  namespace Traversal
  {
    // Instance
    //
    //
    void Instance::
    traverse (Type& a)
    {
      pre (a);
      belongs (a);
      post (a);
    }

    void Instance::
    pre (Type&)
    {
    }

    void Instance::
    belongs (Type& a, EdgeDispatcherBase& d)
    {
      d.dispatch (a.belongs ());
    }

    void Instance::
    belongs (Type& a)
    {
      belongs (a, edge_traverser ());
    }

    void Instance::
    post (Type&)
    {
    }
  }
}
