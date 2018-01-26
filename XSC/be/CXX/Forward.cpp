// file      : CXX/Forward.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "Forward.hpp"
#include "Elements.hpp"

#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

namespace
{
  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      if (e.named ())
        os << "class " << id (e.name ()) << ";";
      else
        os << "class " << type_name (e) << ";";
    }

    string name_;
  };

  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      if (e.named ())
        os << "class " << id (e.name ()) << ";";
      else
        os << "class " << type_name (e) << ";";
    }
  };
}

void
generate_forward (Context& ctx, SemanticGraph::Schema& schema)
{
  ctx.os << "// Forward declarations." << endl;

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
