// file      : CXX/WriterSource.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "WriterSource.hpp"
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
      //string scope (id (e.scope ().name ()));
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
          L << L"(\"" << name << L"\")" << ", " <<
          (q ? L + L"(\"" + ns + L"\")" + L", " : L"") <<
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
          L << L"(\"" << name << L"\")" << ", " <<
          (q ? L + L"(\"" + ns + L"\")" + L", " : L"") <<
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
      //if (!a.type ().named ())
      //  return;

      string name (a.name ());
      //string scope (id (a.scope ().name ()));
      string ns (xs_ns_name (a));

      bool q (a.qualified ());

      os << "void " << scope << "::" << endl
         << id (name) << " (Type const& o)"
         << "{"
         << xml_attribute_type << " a (" <<
        L << L" (\"" << name << L"\")" << ", " <<
        (q ? L + L" (\"" + ns + L"\")" + L", " : L"") <<
        L << L" (\"" << L"\")" << ", " << "top_ ());"
#if defined (ACE_HAS_CPP11)
         << "attr_ (std::addressof(a));"
#else
         << "attr_ (&a);"
#endif
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
      os << "::XMLSchema::Writer::IDREF<" << char_type << ">" <<
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
    Complex (Context& c,
             Traversal::NodeDispatcher& anonymous_type,
             string const& name = L"")
        : Context (c),
          name_ (name),
          member_ (c),
          base__ (c)
    {
      edge_traverser (names_);
      names_.node_traverser (member_);

      base_.node_traverser (base__);

      names_anonymous_.node_traverser (anonymous_type);
    }

    virtual void
    traverse (Type& c)
    {
      if (c.named ())
        name_ = id (c.name ());

      if (!name_.empty ())
      {
        enter_scope (name_);

        c.context ().set ("name", name_);

        // Go after anonymous types first.
        //
        names (c, names_anonymous_);

        Traversal::Complex::traverse (c);

        c.context ().remove ("name");

        leave_scope ();
      }
    }

    virtual void
    pre (Type& c)
    {
      string name (id (name_));

      os << "// " << name << endl;

      // c-tor (XML::Element&)
      //
      os << scope << "::" << endl
         << name << " (" << xml_element_type << "& e)" << endl
         << ": ::XSCRT::Writer<" << char_type << "> (e)" << endl
         << "{"
         << "}";

      // c-tor ()
      //
      os << scope << "::" << endl
         << name << " ()"
         << "{"
         << "}";

      // traverse
      //
      os << "void " << scope << "::" << endl
         << "traverse (Type const& o)"
         << "{";

      inherits (c, base_);

      os << "Traversal::" << scope << "::traverse (o);"
         << "}";
    }

  private:
    string name_;

    Member member_;
    Traversal::Names names_;

    Base base__;
    Traversal::Inherits base_;

    Traversal::Names names_anonymous_;
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
        "s = " << L << " (\"" << name << "\")" << ";"
         << "else ";
    }
  };

  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c, string const & name = L"")
        : Context (c),
          name_ (name),
          enumerator_ (c)
    {
      names_.node_traverser (enumerator_);
    }

    virtual void
    traverse (Type& c)
    {
      string name ((this->name_ != L"") ? id(name_) : id (c.name ()));

      os << "// " << name << endl
         << endl;

      // c-tor (XML::Element&)
      //
      os << name << "::" << endl
         << name << " (" << xml_element_type << "& e)" << endl
         << ": ::XSCRT::Writer<" << char_type << "> (e)" << endl
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
    string name_;
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
  struct WriterBase : /*Traversal::Type, */
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

        os << "virtual " << fq_name (c, L"Writer") << "," << endl;

        Traversal::Complex::traverse (c);
      }
    }

    //virtual void
    //traverse (SemanticGraph::Type& t)
    //{
    //  if (check (t))
    //  {
    //    os << "virtual " << type_name (t, L"Writer") << "," << endl;
    //  }
    //}

    virtual void
    traverse (SemanticGraph::IdRef& r)
    {
      if (check (r))
      {
        os << "virtual ::XMLSchema::Writer::IDREF<" << char_type << ">, " <<
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

        Traversal::Belongs belongs;
        Complex complex_element (*this, *this, name);
        Enumeration enumeration (*this, name);

        belongs.node_traverser (complex_element);

        t.context ().set ("seen", true);

        Element::belongs (e, belongs);

        t.context ().remove ("seen");
      }
    }
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
      os << "namespace writer"
         << "{"
         << "void" << endl
         << id (name) << " (" << fq_name (t) << " const& s, xercesc::DOMDocument* d)"
         << "{"
         << "xercesc::DOMElement* dom_element = d->getDocumentElement ();"
         << "if (!dom_element)"
         << "{"
         << "throw 1;"
         << "}"
         << xml_element_type << " e (dom_element);"
         << "if (e.name () != " << L << " (\"" << name << "\"))"
         << "{"
         << "throw 1;"
         << "}";

      os << "struct W : ";

      WriterBase base (*this);
      base.dispatch (t);

      os << "virtual ::XSCRT::Writer<" << char_type << ">" << endl
         << "{"
         << "W (" << xml_element_type << "& e)" << endl
         << ": ";

      os << "::XSCRT::Writer<" << char_type << "> (e)"
         << "{"
         << "}"
         << "};"
         << "W w (e);"
         << "w.dispatch (s);"
         << "}"
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
    AnonymousType anonymous (ctx);
    Complex complex (ctx, anonymous);
    Enumeration enumeration (ctx);

    ns.edge_traverser (names);
    names.node_traverser (complex);
    names.node_traverser (enumeration);
    names.node_traverser (anonymous);

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
