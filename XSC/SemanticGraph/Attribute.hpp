// file      : XSC/SemanticGraph/Attribute.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_SEMANTIC_GRAPH_ATTRIBUTE_HXX
#define XSC_SEMANTIC_GRAPH_ATTRIBUTE_HXX

#include <XSC/SemanticGraph/Element.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    class Attribute : public virtual Instance
    {
    public:
      bool
      optional () const
      {
        return optional_;
      }

      bool
      qualified () const
      {
        return qualified_;
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Attribute (bool optional, bool qualified)
          : optional_ (optional), qualified_ (qualified)
      {
        type_info (static_type_info ());
      }

    private:
      bool optional_;
      bool qualified_;
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_ATTRIBUTE_HXX
