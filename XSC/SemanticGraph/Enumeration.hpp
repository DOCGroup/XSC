// file      : XSC/SemanticGraph/Enumeration.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_SEMANTIC_GRAPH_ENUMERATION_HPP
#define XSC_SEMANTIC_GRAPH_ENUMERATION_HPP

#include <XSC/SemanticGraph/Elements.hpp>
#include <XSC/SemanticGraph/Complex.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    class Enumeration : public virtual Complex
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Enumeration ()
      {
        type_info (static_type_info ());
      }
    };


    class Enumerator : public virtual Instance
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Enumerator ()
      {
        type_info (static_type_info ());
      }
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_ENUMERATION_HPP
