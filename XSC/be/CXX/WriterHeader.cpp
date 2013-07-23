// file      : CXX/WriterHeader.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "WriterHeader.hpp"
#include "Elements.hpp"

#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

#if defined (_WINDOWS)
# if defined (min)
#   undef min
# endif
# if defined (max)
#   undef max
# endif
#endif

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
      //if (!e.type ().named ())
      //  return;

      string name (e.name ());

      if (e.min () == 0 && e.max () == 1)
      {
        // Borland post
        os << "virtual void " << endl
           << id (name)  << " (Type &o)" << endl
           << "{"
           << "this->" << id (name) << " ("
           << "const_cast <Type const &> (o));" << endl
           << "}";

        // optional
        //
        os << "virtual void" << endl
           << id (name) << " (Type const&);"
           << endl;
      }
      else if (e.min () == 1 && e.max () == 1)
      {
        // Borland post
        os << "virtual void " << endl
           << id (name)  << " (Type &o)" << endl
           << "{"
           << "this->" << id (name) << " ("
           << "const_cast <Type const &> (o));" << endl
           << "}";

        // one
        //
        os << "virtual void" << endl
           << id (name) << " (Type const&);"
           << endl;
      }
      else
      {
        // sequence
        //

        // Pre
        //
        os << "virtual void " << endl
           << name << "_pre (Type &o)" << endl
           << "{"
           << "this->"  << name << "_pre ("
           << "const_cast <Type const &> (o));" << endl
           << "}";

        // pre
        //
        os << "virtual void" << endl
           << name << "_pre (Type const&);"
           << endl;

        // non-const next
        //
        os << "virtual void " << endl
           << name << "_next (Type &o)" << endl
           << "{"
           << "this->"  << name << "_next ("
           << "const_cast <Type const &> (o));" << endl
           << "}";

        // next
        //
        os << "virtual void" << endl
           << name << "_next (Type const&);"
           << endl;

        //  post
        os << "virtual void " << endl
           << name << "_post (Type &o)" << endl
           << "{"
           << "this->" << name << "_post ("
           << "const_cast <Type const &> (o));" << endl
           << "}";

        // post
        //
        os << "virtual void" << endl
           << name << "_post (Type const&);"
           << endl;
      }
    }

    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      // Anonymous types cannot be traversed.
      //
      //if (!a.type ().named ())
      //return;

      string name (a.name ());

      // Borland post
      os << "virtual void " << endl
         << id (name)  << " (Type &o)" << endl
         << "{"
         << "this->" << id (name) << " ("
         << "const_cast <Type const &> (o));" << endl
         << "}";


      os << "virtual void" << endl
         << id (name) << " (Type const&);"
         << endl;
    }
  };

  struct Base : // Traversal::Type,
                Fundamental,
                protected virtual Context
  {
    Base (Context& c)
        : Context (c)
    {
    }

    //virtual void
    //traverse (SemanticGraph::Type& t)
    //{
    //  string type (type_name (t, L"Writer"));

    //  os << "virtual " << type << "," << endl;
    //}

    virtual void
    traverse (SemanticGraph::IdRef&)
    {
      os << "virtual ::XMLSchema::Writer::IDREF< " << char_type << " >, " <<
        endl;
    }

    virtual void
    fundamental_type (SemanticGraph::FundamentalType & t)
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
    Complex (Context& c,
             Traversal::NodeDispatcher& anonymous_type,
             string const& name = L"")
        : Context (c),
          e (c.esymbol.empty () ? c.esymbol : c.esymbol + L" "),
          name_ (name),
          member_ (c),
          base_ (c)
    {
      edge_traverser (names_);
      names_.node_traverser (member_);
      names_.node_traverser (anonymous_type);

      inherits_.node_traverser (base_);
    }

    virtual void
    traverse (Type & c)
    {
      // We need to get the name of the element. The name would
      // be already set if it was an anonymous type.
      if (c.named ())
        name_ = id (c.name ());

      // We only continue if we have a valid name.
      if (!name_.empty ())
      {
        enter_scope (name_);

        Traversal::Complex::traverse (c);

        leave_scope ();
      }
    }

    virtual void
    pre (Type& c)
    {
      os << "struct " << name_ << " : Traversal::" << scope << ", " << endl;

      inherits (c, inherits_);

      os << "virtual ::XSCRT::Writer< " << char_type << " >"
         << "{";

      //@@ Should probably be Type__
      //
      os << "typedef " << fq_name (c) << " Type;";

      // c-tor
      //
      os << name_ << " (" << xml_element_type << "&);"
         << endl;

      // Non-const traverse for Borland
      os << "virtual void " << endl
         << "traverse (Type &o)" << endl
         << "{"
         << "this->traverse ("
         << "const_cast <Type const &> (o));"
         << "}";


      // traverse
      //
      os << "virtual void" << endl
         << "traverse (Type const&);"
         << endl;
    }

    virtual void
    post (Type& )
    {
      os << "protected:" << endl
         << name_ << " ();"
         <<"};";
    }

  private:
    string e;
    string name_;

    Member member_;
    Traversal::Names names_;

    Base base_;
    Traversal::Inherits inherits_;
  };

  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c, string const & name = L"")
        : Context (c),
          ex (c.esymbol.empty () ? c.esymbol : c.esymbol + L" "),
          name_ (name)
    {
    }

    virtual void
    traverse (Type& e)
    {
      string name ((this->name_ != L"") ? name_ : id (e.name ()));
      string type (type_name (e));

      if (name == L"") name = L"BAD NAME";

      os << "struct " << name << " : Traversal::" << name << ", " << endl
         << "virtual ::XSCRT::Writer< " << char_type << " >"
         << "{";

      // c-tor
      //
      os << name << " (" << xml_element_type << "&);"
         << endl;

      // Non-const traverse for Borland
      os << "virtual void " << endl
         << "traverse (Type &o)"
         << "{"
         << "this->traverse ("
         << "const_cast <Type const &> (o));"
         << "}";

      // traverse
      //
      os << "virtual void" << endl
         << "traverse (Type const&);"
         << endl;

      os << "protected:" << endl
         << name << " ();"
         << "};";
    }

  private:
    string ex;

    string name_;
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
      string type (fq_name (c.type ()));

      os << "namespace writer"
         << "{"
         << e
         << "void" << endl
         << id (name) << " (" << type << " const&, xercesc::DOMDocument*);"
         << "}"
         << endl;
    }

  private:
    string e;
  };

  struct AnonymousType : Traversal::Element, protected virtual Context
  {
    AnonymousType (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      SemanticGraph::Type& t (e.type ());

      if (!t.named () && !t.context ().count ("seen"))
      {
        string name (type_name (e));

        os << "// anonymous type for " /* << scope << "::"*/ << name << endl
           << "//" << endl;

        if (dynamic_cast<SemanticGraph::Type*> (&e.scope ()))
          os << "public:" << endl;

        Traversal::Belongs belongs;
        Complex complex (*this, *this, id (name));
        Enumeration enumeration (*this, id (name));

        belongs.node_traverser (complex);
        belongs.node_traverser (enumeration);

        t.context ().set ("seen", true);

        Element::belongs (e, belongs);

        t.context ().remove ("seen");
      }
    }
  };
}

void
generate_writer_header (Context& ctx, SemanticGraph::Schema& schema)
{
  ctx.os << "#include \"ace/XML_Utils/XMLSchema/Writer.hpp\"" << endl
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
    AnonymousType anonymous_type (ctx);
    Complex complex (ctx, anonymous_type);
    Enumeration enumeration (ctx);

    ns.edge_traverser (names);
    names.node_traverser (complex);
    names.node_traverser (enumeration);
    names.node_traverser (anonymous_type);

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
