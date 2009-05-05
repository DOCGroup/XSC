// file      : XSC/Traversal/Complex.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "XSC/Traversal/Complex.hpp"

namespace XSC
{
  namespace Traversal
  {
    void Complex::
    traverse (Type& c)
    {
      pre (c);
      name (c);
      inherits (c);
      names (c);
      read_write_type (c);
      post (c);
      insertion_extraction (c);
    }

    void Complex::
    pre (Type&)
    {
    }

    void Complex::
    name (Type&)
    {
    }

    bool Complex::
    inherits (Type& c, EdgeDispatcherBase& d)
    {
      Type::InheritsIterator b (c.inherits_begin ()), e (c.inherits_end ());

      if (b != e) 
        iterate_and_traverse (b, e, d);

      else return false;

      return true;
    }   

    void Complex::
    inherits (Type& c)
    {
      Type::InheritsIterator b (c.inherits_begin ()), e (c.inherits_end ());

      if (b != e)
      {
        inherits_pre (c);
        iterate_and_traverse (b,
                              e,
                              edge_traverser ()/*,
                              *this,
                              &InterfaceTemplate::comma,
                              c*/);
	
	inherits_post (c);
      }
      else
      {
        inherits_none (c);
      }

    }

    void Complex::
    inherits_pre (Type&)
    {
    }

    void Complex::
    inherits_post (Type&)
    {
    }

    void Complex::
    inherits_none (Type&)
    {
    }

    void Complex::
    post (Type&)
    {
    }

    void Complex::
    read_write_type (Type&)
    {
    }

    void Complex::
    insertion_extraction (Type&)
    {
    }
  }
}
