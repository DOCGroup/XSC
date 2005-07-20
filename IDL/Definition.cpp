// file      : IDL/Definition.cxx
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <IDL/Definition.hpp>
#include <IDL/Elements.hpp>

#include <XSC/SemanticGraph.hpp>
#include <XSC/Traversal.hpp>

#include <CCF/CodeGenerationKit/Regex.hpp>

using namespace IDL;

namespace
{
  struct Base : Traversal::Type,
                Traversal::FundamentalType,
                Traversal::Enumeration,
                protected virtual Context
  {
    Base (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (SemanticGraph::Type& c)
    {
      os << " :  " << type_name (c)
         << "{";
    }

    virtual void
    traverse (SemanticGraph::FundamentalType& c)
    {
      os << " :  " << type_name (c, L"_value")
         << "{";
    }

    virtual void
    traverse (SemanticGraph::Enumeration& c)
    {
      os << " :  " << type_name (c, L"_value")
         << "{";
    }
  };

  struct Element : Traversal::Element, protected virtual Context
  {
    Element (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& c)
    {
      string name (c.name ());
      string type (type_name (c));

      os << "// " << name << endl
         << "// " << endl;

      if (c.max () != 1)
      {
        string name_seq (id (name + L"_seq"));

        // sequence
        //
        os << "typedef sequence< " << type << " > " << name_seq << ";"
           << "public " << name_seq << " " << id (name) << ";";

      }
      else if (c.min () == 0)
      {
        // optional
        //
        os << "boolean " << name << "_p ();"
           << "public " << type << " " << id (name) << ";";
      }
      else
      {
        // one
        //
        os << "public " << type << " " << id (name) << ";";
      }

      os << endl;
    }
  };


  struct Attribute : Traversal::Attribute, protected virtual Context
  {
    Attribute (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& a)
    {
      string name (a.name ());
      string type (type_name (a));

      os << "// " << name << endl
         << "// " << endl;


      if (a.optional ())
      {
        os << "boolean " << name << "_p ();"
           << "public " << type << " " << id (name) << ";";
      }
      else
      {
        os << "public " << type << " " << id (name) << ";";
      }

      os << endl;
    }
  };


  struct Complex : Traversal::Complex,
                   Traversal::Enumeration,
                   protected virtual Context
  {
    Complex (Context& c)
        : Context (c),
          base_ (c),
          element_ (c),
          attribute_ (c)
    {
      edge_traverser (inherits_);
      edge_traverser (names_);

      inherits_.node_traverser (base_);
      names_.node_traverser (element_);
      names_.node_traverser (attribute_);
    }

    virtual void
    pre (SemanticGraph::Complex& c)
    {
      os << "valuetype " << id (c.name ());
    }

    virtual void
    inherits_none (SemanticGraph::Complex&)
    {
      os << " : ::XMLSchema::object_"
         << "{";
    }

    virtual void
    post (SemanticGraph::Complex&)
    {
      os << "};";
    }

    virtual void
    traverse (SemanticGraph::Enumeration&)
    {
    }

  private:
    string name;
    string e;

    Traversal::Inherits inherits_;
    Traversal::Names names_;

    Base base_;
    Element element_;
    Attribute attribute_;
  };

  struct Includes : Traversal::Imports,
                    Traversal::Includes,
                    protected virtual Context
  {
    Includes (Context& c, std::string const& expr)
        : Context (c), expr_ (expr)
    {
    }

    virtual void
    traverse (SemanticGraph::Imports& i)
    {
      traverse (i.file ());
    }

    virtual void
    traverse (SemanticGraph::Includes& i)
    {
      traverse (i.file ());
    }

    virtual void
    traverse (fs::path const& f)
    {
      std::string name (regex::perl_s (f.string (), expr_));

      os << "#include \"" << name.c_str () << "\"" << endl
         << endl;
    }

  private:
    std::string expr_;
  };
}

void IDL::
generate_definition (Context& ctx,
                     SemanticGraph::Schema& schema,
                     std::string const& expr)
{
  ctx.os << "#include " << (ctx.char_type == L"wchar"
                            ? "<XMLSchema/WideTypes.idl>"
                            : "<XMLSchema/NarrowTypes.idl>") << endl
         << endl;


  Traversal::Schema traverser;

  Traversal::Sources sources;
  Includes includes (ctx, expr);
  Traversal::Names schema_names;

  Namespace ns (ctx);

  traverser.edge_traverser (sources);
  traverser.edge_traverser (includes);
  traverser.edge_traverser (schema_names);
  sources.node_traverser (traverser);
  schema_names.node_traverser (ns);

  Traversal::Names names;

  Complex complex (ctx);

  ns.edge_traverser (names);
  names.node_traverser (complex);

  traverser.traverse (schema);
}
