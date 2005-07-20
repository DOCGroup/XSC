// file      : IDL/Declaration.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <IDL/Declaration.hpp>
#include <IDL/Elements.hpp>

#include <XSC/SemanticGraph.hpp>
#include <XSC/Traversal.hpp>

using namespace IDL;

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
      os << "valuetype " << id (e.name ()) << ";";
    }
  };

  struct Enumerator : Traversal::Enumerator, protected virtual Context
  {
    Enumerator (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
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

    virtual void
    pre (Type& e)
    {
      os << "enum " << id (e.name ())
         << "{";
    }

    virtual void
    names_next (Type&)
    {
      os << "," << endl;
    }

    virtual void
    post (Type&)
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
  ctx.os << "// Forward declarations." << endl
         << "//" << endl;

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
