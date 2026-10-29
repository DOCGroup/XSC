// file      : IDL/Declaration.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "Declaration.hpp"
#include "Elements.hpp"
#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

using namespace IDL;

namespace
{
  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c)
        : Context (c)
    {
    }

    void
    traverse (Type& e) override
    {
      os << "valuetype " << id (e.name ()) << ";";
    }
  };

  struct Enumerator : Traversal::Enumerator, protected virtual Context
  {
    Enumerator (Context& c)
        : Context (c)
    {
    }

    void
    traverse (Type& e) override
    {
      os << id (e.name ());
    }
  };

  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c)
        : Context (c), enumerator_ (c)
    {
      edge_traverser (names_);
      names_.node_traverser (enumerator_);
    }

    void
    pre (Type& e) override
    {
      os << "enum " << id (e.name ())
         << "{";
    }

    void
    names_next (Type&)
    {
      os << "," << std::endl;
    }

    void
    post (Type&) override
    {
      os << "};";
    }

  private:
    Enumerator enumerator_;
    Traversal::Names names_;
  };
}

void IDL::
generate_declaration (Context& ctx, SemanticGraph::Schema& schema)
{
  ctx.os << "// Forward declarations." << std::endl;

  Traversal::Schema traverser;
  Traversal::Sources sources;
  Traversal::Names schema_names;
  Namespace ns (ctx);

  traverser.edge_traverser (sources);
  traverser.edge_traverser (schema_names);
  sources.node_traverser (traverser);
  schema_names.node_traverser (ns);

  Traversal::Names names;
  Complex complex (ctx);
  Enumeration enumeration (ctx);

  ns.edge_traverser (names);
  names.node_traverser (complex);
  names.node_traverser (enumeration);

  traverser.traverse (schema);
}
