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

      string compare_name;
      if (char_type == L"wchar_t")
        {
          compare_name = L"L\"" + name + L"\"";
        }
      else if (char_type == L"ACE_TCHAR")
        {
          compare_name = L"ACE_TEXT(\"" + name + L"\")";
        }
      else
        {
          compare_name = L"\"" + name + L"\"";
        }

      os << "namespace reader"
         << "{"
         << type << endl
         << id (name) << " (xercesc::DOMDocument const* d)"
         << "{";

      if (this->cppmode_ == CPPMODE::CPP03)
      {
         os << "// Initiate our Singleton as an ACE_TSS object (ensures thread" << endl
            << "// specific storage" << endl
            << "ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());";
      }

      os << "xercesc::DOMElement* dom_element = d->getDocumentElement ();"
         << "if (!dom_element)"
         << "{"
         << "throw 1;"
         << "}"
         << xml_element_type << " e (dom_element);"
         << "if (e.name () == " << compare_name << ")"
         << "{"
         << type << " r (e);\n";

     if (this->cppmode_ == CPPMODE::CPP03)
     {
       os  << "(*TSS_ID_Map)->resolve_idref();" << endl;
     }
     os  << "return r;"
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
