// file      : CXX/WriterHeader.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <CXX/WriterHeader.hpp>
#include <CXX/Elements.hpp>

#include <XSC/SemanticGraph.hpp>
#include <XSC/Traversal.hpp>

namespace
{
  struct Member : Traversal::Element,
                  Traversal::Attribute,
                  protected virtual Context
  {
    Member (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (SemanticGraph::Element& e)
    {
      // Anonymous types cannot be traversed.
      //
      if (!e.type ().named ()) return;

      string name (e.name ());

      if (e.max () != 1)
      {
        // sequence
        //

        // pre
        //
        os << "virtual void" << endl
           << name << "_pre (Type const&);"
           << endl;

        // next
        //
        os << "virtual void" << endl
           << name << "_next (Type const&);"
           << endl;


        // post
        //
        os << "virtual void" << endl
           << name << "_post (Type const&);"
           << endl;
      }
      else if (e.min () == 0)
      {
        // optional
        //
        os << "virtual void" << endl
           << id (name) << " (Type const&);"
           << endl;
      }
      else
      {
        // one
        //
        os << "virtual void" << endl
           << id (name) << " (Type const&);"
           << endl;
      }
    }

    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      // Anonymous types cannot be traversed.
      //
      if (!a.type ().named ()) return;

      string name (a.name ());

      os << "virtual void" << endl
         << id (name) << " (Type const&);"
         << endl;
    }
  };

  struct Base : Traversal::Type,
                Fundamental,
                protected virtual Context
  {
    Base (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (SemanticGraph::Type& t)
    {
      string type (type_name (t, L"Writer"));

      os << "virtual " << type << "," << endl;
    }

    virtual void
    traverse (SemanticGraph::IdRef& r)
    {
      os << "virtual ::XMLSchema::Writer::IDREF< " << char_type << " >, " <<
        endl;
    }

    virtual void
    fundamental_type (SemanticGraph::FundamentalType& t)
    {
      string name (id (t.name ()));

      os << "virtual ::XMLSchema::Writer::FundamentalType< " <<
        "::XMLSchema::" << name << ", " << char_type << " >," << endl;
    }

    void
    fundamental_template (SemanticGraph::FundamentalType& t)
    {
      string name (id (t.name ()));

      os << "virtual ::XMLSchema::Writer::FundamentalType< " <<
        "::XMLSchema::" << name << "< " << char_type << " >, " <<
        char_type << " >," << endl;
    }
  };

  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c)
        : Context (c),
          e (c.esymbol.empty () ? c.esymbol : c.esymbol + L" "),
          member_ (c),
          base_ (c)
    {
      edge_traverser (names_);
      names_.node_traverser (member_);

      inherits_.node_traverser (base_);
    }

    virtual void
    pre (Type& c)
    {
      string name (id (c.name ()));
      string type (type_name (c));

      os << "struct " << e << name << " : Traversal::" << name << ", " << endl;

      inherits (c, inherits_);

      os << "virtual ::XSCRT::Writer< " << char_type << " >"
         << "{";

      //@@ Should probably be Type__
      //
      os << "typedef " << type << " Type;";

      // c-tor
      //
      os << name << " (" << xml_element_type << "&);"
         << endl;

      // traverse
      //
      os << "virtual void" << endl
         << "traverse (Type const&);"
         << endl;
    }

    virtual void
    post (Type& c)
    {
      string name (id (c.name ()));

      os << "protected:" << endl
         << name << " ();"
         <<"};";
    }

  private:
    string e;
    Member member_;
    Traversal::Names names_;

    Base base_;
    Traversal::Inherits inherits_;
  };

  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c)
        : Context (c),
          ex (c.esymbol.empty () ? c.esymbol : c.esymbol + L" ")
    {
    }

    virtual void
    traverse (Type& e)
    {
      string name (id (e.name ()));
      string type (type_name (e));

      os << "struct " << ex << name << " : Traversal::" << name << ", " << endl
         << "virtual ::XSCRT::Writer< " << char_type << " >"
         << "{";

      // c-tor
      //
      os << name << " (" << xml_element_type << "&);"
         << endl;

      // traverse
      //
      os << "virtual void" << endl
         << "traverse (Type const&);"
         << endl;

      os << "protected:" << endl
         << name << " ();"
         <<"};";
    }

  private:
    string ex;
  };


  struct WriterNamespace : Namespace
  {
    WriterNamespace (Context& c)
        : Context (c), Namespace (c)
    {
    }

    virtual void
    pre (Type& n)
    {
      Namespace::pre (n);

      os << "namespace Writer"
         << "{";
    }

    virtual void
    post (Type& n)
    {
      os << "}";

      Namespace::post (n);
    }
  };

  struct Element : Traversal::Element, protected virtual Context
  {
    Element (Context& c)
        : Context (c),
          e (c.esymbol.empty () ? c.esymbol : c.esymbol + L"\n")
    {
    }

    virtual void
    traverse (Type& c)
    {
      string name (c.name ());
      string type (type_name (c));

      os << e
         << "void" << endl
         << id (name) << " (" << type << " const&, xercesc::DOMDocument*);"
         << endl;
    }

  private:
    string e;
  };
}

void
generate_writer_header (Context& ctx, SemanticGraph::Schema& schema)
{
  ctx.os << "#include \"XMLSchema/Writer.hpp\"" << endl
         << endl;

  {
    Traversal::Schema traverser;
    Traversal::Sources sources;
    Traversal::Names schema_names;
    WriterNamespace ns (ctx);

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
    Element element (ctx);
    ns.edge_traverser (names);
    names.node_traverser (element);

    traverser.traverse (schema);
  }
}
