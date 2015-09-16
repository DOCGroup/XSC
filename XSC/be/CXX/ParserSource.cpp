// file      : CXX/ParserSource.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "ParserSource.hpp"
#include "Elements.hpp"

#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

namespace
{
  struct Parser : Traversal::Element, protected virtual Context
  {
    Parser (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& c)
    {
      string name (c.name ());
      string type (type_name (c));

      //@@ need to use FQ-names.
      //
      os << "namespace reader"
         << "{"
         << type << endl
         << id (name) << " (xercesc::DOMDocument const* d)"
         << "{"
         <<  "//Initiate our Singleton as an ACE_TSS object (ensures thread\n"
         <<  "//specific storage\n"
         <<  "ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());\n\n"
         << xml_element_type << " e (d->getDocumentElement ());"
         << "if (e.name () == ACE_TEXT(\"" << name << "\"))"
         << "{"
         << type << " r (e);\n"
         << "(*TSS_ID_Map)->resolve_idref();\n"
         << "return r;"
         << "}"
         << "else"
         << "{"
         << "throw 1;"
         << "}"
         << "}"
         << "}";
    }
  };
}

void
generate_parser_source (Context& ctx, SemanticGraph::Schema& schema)
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
