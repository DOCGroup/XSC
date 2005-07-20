// file      : XSC/SemanticGraph/Element.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef XSC_SEMANTIC_GRAPH_ELEMENT_HXX
#define XSC_SEMANTIC_GRAPH_ELEMENT_HXX

#include <XSC/SemanticGraph/Elements.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    class Element : public virtual Instance
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

      unsigned long
      min () const
      {
        return min_;
      }

      unsigned long
      max () const
      {
        return max_;
      }

      bool
      qualified () const
      {
        return qualified_;
      }

    protected:
      friend class Graph<Node, Edge>;

      Element (unsigned long min, unsigned long max, bool qualified)
          : min_ (min), max_ (max), qualified_ (qualified)
      {
        type_info (static_type_info ());
      }

    private:
      unsigned long min_, max_;
      bool qualified_;
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_ELEMENT_HXX
