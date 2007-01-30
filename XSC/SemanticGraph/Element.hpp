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
    /// This class represents an xsd:element in the semantic graph.
    class Element : public virtual Instance
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

      unsigned long
      min () const
      {
        return this->min_;
      }

      unsigned long
      max () const
      {
        return this->max_;
      }

      bool
      qualified () const
      {
        return this->qualified_;
      }
      
      bool
      href () const
      {
        return this->href_;
      }
      
    protected:
      friend class Graph<Node, Edge>;

      Element (unsigned long min, unsigned long max, bool qualified)
          : min_ (min), max_ (max), qualified_ (qualified)
      {
        type_info (static_type_info ());
      }

    private:
      /// The minimum cardinality of the element.
      unsigned long min_;
      /// The maximum cardinality of the element
      unsigned long max_;
      /// ?
      bool qualified_;
      
      /// true if this element can be specified using an xmi href reference
      bool href_;
      
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_ELEMENT_HXX
