// file      : XSC/SemanticGraph/Namespace.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_SEMANTIC_GRAPH_NAMESPACE_HXX
#define XSC_SEMANTIC_GRAPH_NAMESPACE_HXX

#include <XSC/SemanticGraph/Elements.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    class Namespace : public virtual Scope
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Namespace ()
      {
        type_info (static_type_info ());
      }
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_NAMESPACE_HXX
