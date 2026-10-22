// file      : XSC/Traversal/Schema.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_TRAVERSAL_SCHEMA_HPP
#define XSC_TRAVERSAL_SCHEMA_HPP

#include "XSC/Traversal/Elements.hpp"
#include "XSC/SemanticGraph/Schema.hpp"

namespace XSC
{
  namespace Traversal
  {
    //
    //
    //
    struct Implies : Edge<SemanticGraph::Implies>
    {
      void
      traverse (Type& e) override
      {
        this->node_traverser ().dispatch (e.element ());
      }
    };

    //
    //
    //
    struct Sources : Edge<SemanticGraph::Sources>
    {
      void
      traverse (Type& e) override
      {
        this->node_traverser ().dispatch (e.element ());
      }
    };


    //
    //
    //
    struct Includes : Edge<SemanticGraph::Includes>
    {
      void
      traverse (Type& e) override
      {
        this->node_traverser ().dispatch (e.element ());
      }
    };


    //
    //
    //
    struct Imports : Edge<SemanticGraph::Imports>
    {
      void
      traverse (Type& e) override
      {
        this->node_traverser ().dispatch (e.element ());
      }
    };


    //
    //
    //
    struct Schema : ScopeTemplate<SemanticGraph::Schema>
    {
      void
      traverse (Type& s) override
      {
        pre (s);

        this->iterate_and_traverse (s.contains_begin (),
                                    s.contains_end (),
                                    edge_traverser ());

        this->names (s);

        post (s);
      }

      virtual void
      pre (Type&)
      {
      }

      virtual void
      post (Type&)
      {
      }
    };
  }
}

#endif  // XSC_TRAVERSAL_SCHEMA_HPP
