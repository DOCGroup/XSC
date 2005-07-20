// file      : XSC/SemanticGraph/Complex.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef XSC_SEMANTIC_GRAPH_COMPLEX_HXX
#define XSC_SEMANTIC_GRAPH_COMPLEX_HXX

#include <XSC/SemanticGraph/Elements.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    class Complex : public virtual Type, public virtual Scope
    {
      typedef
      std::vector <Inherits*>
      Inherits_;

    public:
      typedef
      Inherits_::const_iterator
      InheritsIterator;

      InheritsIterator
      inherits_begin () const
      {
        return inherits_.begin ();
      }

      InheritsIterator
      inherits_end () const
      {
        return inherits_.end ();
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Complex ()
      {
        type_info (static_type_info ());
      }

      using Type::add_edge_right;
      using Scope::add_edge_left;

      void
      add_edge_left (Inherits& e)
      {
        inherits_.push_back (&e);
      }

    private:
      Inherits_ inherits_;
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_COMPLEX_HXX
