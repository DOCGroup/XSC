// file      : CXX/WriterSource.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <CXX/WriterSource.hpp>
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
      string scope (id (e.scope ().name ()));
      string ns (xs_ns_name (e));

      bool q (e.qualified ());

      if (e.max () != 1)
      {
        // sequence
        //

        // pre
        //
        os << "void " << scope << "::" << endl
           << name << "_pre (Type const&)"
           << "{"
           << "push_ (" << xml_element_type << " (" <<
          L << '"' << name << '"' << ", " <<
          (q ? L + L'"' + ns + L'"' + L", " : L"") <<
          "top_ ()));"
           << "}";

        // next
        //
        os << "void " << scope << "::" << endl
           << name << "_next (Type const& o)"
           << "{"
           << name << "_post (o);"
           << name << "_pre (o);"
           << "}";


        // post
        //
        os << "void " << scope << "::" << endl
           << name << "_post (Type const&)"
           << "{"
           << "pop_ ();"
           << "}";
      }
      else
      {
        // optional & one
        //
        os << "void " << scope << "::" << endl
           << id (name) << " (Type const& o)"
           << "{"
           << "push_ (" << xml_element_type << " (" <<
          L << '"' << name << '"' << ", " <<
          (q ? L + L'"' + ns + L'"' + L", " : L"") <<
          "top_ ()));"
           << "Traversal::" << scope << "::" << id (name) << " (o);"
           << "pop_ ();"
           << "}";
      }
    }

    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      // Anonymous types cannot be traversed.
      //
      if (!a.type ().named ()) return;

      string name (a.name ());
      string scope (id (a.scope ().name ()));
      string ns (xs_ns_name (a));

      bool q (a.qualified ());

      os << "void " << scope << "::" << endl
         << id (name) << " (Type const& o)"
         << "{"
         << xml_attribute_type << " a (" <<
        L << '"' << name << '"' << ", " <<
        (q ? L + L'"' + ns + L'"' + L", " : L"") <<
        L << '"' << '"' << ", " << "top_ ());"
         << "attr_ (&a);"
         << "Traversal::" << scope << "::" << id (name) << " (o);"
         << "attr_ (0);"
         << "}";
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

      os << type << "::traverse (o);";
    }

    virtual void
    traverse (SemanticGraph::IdRef&)
    {
      os << "::XMLSchema::Writer::IDREF< " << char_type << " >" <<
        "::traverse (o);";
    }

    virtual void
    fundamental_type (SemanticGraph::FundamentalType& t)
    {
      string name (id (t.name ()));

      os << "::XMLSchema::Writer::FundamentalType< " <<
        "::XMLSchema::" << name << ", " << char_type << " >::traverse (o);";
    }

    void
    fundamental_template (SemanticGraph::FundamentalType& t)
    {
      string name (id (t.name ()));

      os << "::XMLSchema::Writer::FundamentalType< " <<
        "::XMLSchema::" << name << "< " << char_type << " >, " <<
        char_type << " >::traverse (o);";
    }
  };

  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c)
        : Context (c), member_ (c), base__ (c)
    {
      edge_traverser (names_);
      names_.node_traverser (member_);

      base_.node_traverser (base__);
    }

    virtual void
    pre (Type& c)
    {
      string name (id (c.name ()));

      os << "// " << name << endl
         << "//" << endl
         << "//" << endl
         << endl;

      // c-tor (XML::Element&)
      //
      os << name << "::" << endl
         << name << " (" << xml_element_type << "& e)" << endl
         << ": ::XSCRT::Writer< " << char_type << " > (e)" << endl
         << "{"
         << "}";

      // c-tor ()
      //
      os << name << "::" << endl
         << name << " ()"
         << "{"
         << "}";      

      // traverse
      //
      os << "void " << name << "::" << endl
         << "traverse (Type const& o)"
         << "{";

      inherits (c, base_);

      os << "Traversal::" << name << "::traverse (o);"
         << "}";
    }

  private:
    Member member_;
    Traversal::Names names_;

    Base base__;
    Traversal::Inherits base_;
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
      string name (e.name ());
      string type (type_name (e));

      os << "if (o == " << type << "::" << id (name) << ") " <<
        "s = " << L << '"' << name << '"' << ";"
         << "else ";
    }
  };

  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c)
        : Context (c), enumerator_ (c)
    {
      names_.node_traverser (enumerator_);
    }

    virtual void
    traverse (Type& c)
    {
      string name (id (c.name ()));

      os << "// " << name << endl
         << "//" << endl
         << "//" << endl
         << endl;

      // c-tor (XML::Element&)
      //
      os << name << "::" << endl
         << name << " (" << xml_element_type << "& e)" << endl
         << ": ::XSCRT::Writer< " << char_type << " > (e)" << endl
         << "{"
         << "}";

      // c-tor ()
      //
      os << name << "::" << endl
         << name << " ()"
         << "{"
         << "}";

      // traverse
      //
      os << "void " << name << "::" << endl
         << "traverse (Type const& o)"
         << "{"
         << string_type << " s;"
         << endl;

      names (c, names_);

      os << "{}"
         << "if (::XSCRT::XML::Attribute< " << char_type << " >* a = attr_ ())"
         << "{"
         << "a->value (s);"
         << "}"
         << "else"
         << "{"
         << "top_().value (s);"
         << "}"
         << "}";
    }

  private:
    Enumerator enumerator_;
    Traversal::Names names_;
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

  //
  //
  //
  struct WriterBase : Traversal::Type,
                      Fundamental,
                      Traversal::Complex,
                      Traversal::Element,
                      Traversal::Attribute,
                      protected virtual Context
  {
    WriterBase (Context& c)
        : Context (c)
    {
      names_.node_traverser (*this);
      belongs_.node_traverser (*this);

      edge_traverser (names_);
      edge_traverser (belongs_);
    }

    virtual void
    traverse (SemanticGraph::Complex& c)
    {
      if (check (c))
      {
        os << "virtual " << type_name (c, L"Writer") << "," << endl;

        Traversal::Complex::traverse (c);
      }
    }

    virtual void
    traverse (SemanticGraph::Type& t)
    {
      if (check (t))
      {
        os << "virtual " << type_name (t, L"Writer") << "," << endl;
      }
    }

    virtual void
    traverse (SemanticGraph::IdRef& r)
    {
      if (check (r))
      {
        os << "virtual ::XMLSchema::Writer::IDREF< " << char_type << " >, " <<
          endl;
      }
    }

    virtual void
    fundamental_type (SemanticGraph::FundamentalType& t)
    {
      if (check (t))
      {
        string name (id (t.name ()));

        os << "virtual ::XMLSchema::Writer::FundamentalType< " <<
          "::XMLSchema::" << name << ", " << char_type << " >," << endl;
      }
    }

    void
    fundamental_template (SemanticGraph::FundamentalType& t)
    {
      if (check (t))
      {
        string name (id (t.name ()));

        os << "virtual ::XMLSchema::Writer::FundamentalType< " <<
          "::XMLSchema::" << name << "< " << char_type << " >, " <<
          char_type << " >," << endl;
      }
    }

  private:
    bool
    check (SemanticGraph::Type& t)
    {
      if (types_.find (&t) == types_.end ())
      {
        types_.insert (&t);
        return true;
      }
      else
      {
        return false;
      }
    }


  private:
    Traversal::Names names_;
    Traversal::Belongs belongs_;

    std::set<SemanticGraph::Type*> types_;
  };


  struct Writer : Traversal::Element, protected virtual Context
  {
    Writer (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& c)
    {
      string name (c.name ());
      string type (type_name (c));
      string wtype (type_name (c, L"Writer"));

      SemanticGraph::Type& t (c.type ());

      //@@ need to use FQ-names.
      //
      os << "void" << endl
         << id (name) << " (" << type << " const& s, xercesc::DOMDocument* d)"
         << "{"
         << xml_element_type << " e (d->getDocumentElement ());"
         << "if (e.name () != " << L << "\"" << name << "\")"
         << "{"
         << "throw 1;"
         << "}";

      os << "struct W : ";

      WriterBase base (*this);
      base.dispatch (t);

      os << "virtual ::XSCRT::Writer< " << char_type << " >" << endl
         << "{"
         << "W (" << xml_element_type << "& e)" << endl
         << ": ";

      os << "::XSCRT::Writer< " << char_type << " > (e)"
         << "{"
         << "}"
         << "};"
         << "W w (e);"
         << "w.dispatch (s);"
         << "}";
    }
  };
}

void
generate_writer_source (Context& ctx, SemanticGraph::Schema& schema)
{
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
    Writer writer (ctx);
    ns.edge_traverser (names);
    names.node_traverser (writer);

    traverser.traverse (schema);
  }
}
