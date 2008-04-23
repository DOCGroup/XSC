// file      : CXX/ParserHeader.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <CXX/ParserHeader.hpp>
#include <CXX/Elements.hpp>

#include <XSC/SemanticGraph.hpp>
#include <XSC/Traversal.hpp>

namespace
{
  struct Parser : Traversal::Element, protected virtual Context
  {
    Parser (Context& c)
        : Context (c), e (c.esymbol.empty () ? c.esymbol : c.esymbol + L"\n")
    {
    }

    virtual void
    traverse (Type& c)
    {
      string name (c.name ());
      string type (type_name (c));

      os << "namespace reader"
         << "{"
         << e
         << type << endl
         << id (name) << " (xercesc::DOMDocument const*);"
         << "}"
         << endl;
    }

  private:
    string e;
  };
}

void
generate_parser_header (Context& ctx, SemanticGraph::Schema& schema)
{

  Traversal::Schema traverser;
  Traversal::Sources sources;
  Traversal::Names schema_names;
  Namespace ns (ctx);

  traverser.edge_traverser (sources);
  traverser.edge_traverser (schema_names);
  sources.node_traverser (traverser);
  schema_names.node_traverser (ns);

  Traversal::Names names;
  Parser parser (ctx);
  ns.edge_traverser (names);
  names.node_traverser (parser);

  traverser.traverse (schema);
}
