// file      : CXX/WriterSource.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "WriterSource.hpp"
#include "Elements.hpp"
#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

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

    void
    traverse (SemanticGraph::Element& e) override
    {
      // Anonymous types cannot be traversed.
      //if (!e.type ().named ())
      //  return;

      string name (e.name ());
      //string scope (id (e.scope ().name ()));
      string ns (xs_ns_name (e));

      bool q (e.qualified ());

      if (e.maximum () != 1)
      {
        // sequence

        // pre
        os << "void " << scope << "::"
           << name << "_pre (Type const&)"
           << "{"
           << "push_ (" << xml_element_type << " (" <<
          L << L"(\"" << name << L"\")" << ", " <<
          (q ? L + L"(\"" + ns + L"\")" + L", " : L"") <<
          "top_ ()));"
           << "}";

        // next
        os << "void " << scope << "::"
           << name << "_next (Type const& o)"
           << "{"
           << name << "_post (o);"
           << name << "_pre (o);"
           << "}";


        // post
        os << "void " << scope << "::"
           << name << "_post (Type const&)"
           << "{"
           << "pop_ ();"
           << "}";
      }
      else
      {
        // optional & one
        os << "void " << scope << "::"
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

    void
    traverse (SemanticGraph::Attribute& a) override
    {
      // Anonymous types cannot be traversed.
      //if (!a.type ().named ())
      //  return;

      string name (a.name ());
      //string scope (id (a.scope ().name ()));
      string ns (xs_ns_name (a));
      std::wstring attrinit;
      if (this->cppmode_ != CPPMODE::CPP03)
      {
        attrinit = L"attr_ (std::addressof(a));";
      }
      else
      {
        attrinit = L"attr_ (&a);";
      }

      bool q (a.qualified ());

      os << "void " << scope << "::"
         << id (name) << " (Type const& o)"
         << "{"
         << xml_attribute_type << " a (" <<
        L << L" (\"" << name << L"\")" << ", " <<
        (q ? L + L" (\"" + ns + L"\")" + L", " : L"") <<
        L << L" (\"" << L"\")" << ", " << "top_ ());"
         << attrinit
         << "Traversal::" << scope << "::" << id (name) << " (o);";

      if (this->cppmode_ != CPPMODE::CPP03)
      {
        os << "attr_ (nullptr);";
      }
      else
      {
        os << "attr_ (0);";
      }

      os << "}";
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

    void
    traverse (SemanticGraph::Type& t) override
    {
      string type (type_name (t, L"Writer"));

      os << type << "::traverse (o);";
    }

    void
    traverse (SemanticGraph::IdRef&) override
    {
      os << "::XMLSchema::Writer::IDREF<" << char_type << ">" <<
        "::traverse (o);";
    }

    void
    fundamental_type (SemanticGraph::FundamentalType& t) override
    {
      string name (fundamental_type_name (t));

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

    void
    traverse (Type& c) override
    {
      if (c.named ())
        name_ = id (c.name ());

      if (!name_.empty ())
      {
        enter_scope (name_);

        c.context ().set ("name", name_);

        // Go after anonymous types first.
        names (c, names_anonymous_);

        Traversal::Complex::traverse (c);

        c.context ().remove ("name");

        leave_scope ();
      }
    }

    void
    pre (Type& c) override
    {
      string name (id (name_));

      // c-tor (XML::Element&)
      os << scope << "::"
         << name << " (" << xml_element_type << "& e)" << std::endl
         << ": ::XSCRT::Writer<" << char_type << "> (e)" << std::endl
         << "{"
         << "}";

      // c-tor ()
      if (this->cppmode_ == CPPMODE::CPP03)
      {
        os << scope << "::"
          << name << " ()"
          << "{"
          << "}";
      }

      // traverse
      os << "void " << scope << "::"
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

    void
    traverse (Type& e) override
    {
      string name (e.name ());
      string type (type_name (e));

      os << "case " << type << "::" << enumerator_name (e);
      if (this->cppmode_ != CPPMODE::CPP17)
        os << "_l";
      os << ": "
         << "s = " << string_literal (name) << "; "
         << "break;";
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

    void
    traverse (Type& c) override
    {
      string name ((this->name_ != L"") ? id(name_) : id (c.name ()));

      // c-tor (XML::Element&)
      os << name << "::"
         << name << " (" << xml_element_type << "& e)" << std::endl
         << ": ::XSCRT::Writer<" << char_type << "> (e)" << std::endl
         << "{"
         << "}";

      // c-tor ()
      if (this->cppmode_ == CPPMODE::CPP03)
      {
        os << name << "::"
          << name << " ()"
          << "{"
          << "}";
      }

      // traverse
      os << "void " << name << "::"
         << "traverse (Type const& o)"
         << "{"
         << string_type << " s;"
         << "switch (o.integral ())"
         << "{";

      names (c, names_);

      os << "default: throw std::invalid_argument (\"invalid " << name << " value\");"
         << "}"
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

    void
    pre (Type& n) override
    {
      Namespace::pre (n);

      os << "namespace Writer"
         << "{";


    }

    void
    post (Type& n) override
    {
      os << "}";

      Namespace::post (n);
    }
  };

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

    void
    traverse (SemanticGraph::Complex& c) override
    {
      if (check (c))
      {

        os << "virtual " << fq_name (c, L"Writer") << "," << std::endl;

        Traversal::Complex::traverse (c);
      }
    }

    //virtual void
    //traverse (SemanticGraph::Type& t)
    //{
    //  if (check (t))
    //  {
    //    os << "virtual " << type_name (t, L"Writer") << "," << std::endl;
    //  }
    //}

    void
    traverse (SemanticGraph::IdRef& r) override
    {
      if (check (r))
      {
        os << "virtual ::XMLSchema::Writer::IDREF<" << char_type << ">, " <<
          std::endl;
      }
    }

    void
    fundamental_type (SemanticGraph::FundamentalType& t) override
    {
      if (check (t))
      {
        string name (fundamental_type_name (t));

        os << "virtual ::XMLSchema::Writer::FundamentalType< " <<
          "::XMLSchema::" << name << ", " << char_type << " >," << std::endl;
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
          char_type << " >," << std::endl;
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

    void
    traverse (Type& e) override
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

    void
    traverse (Type& c) override
    {
      string name (c.name ());
      string type (type_name (c));
      string wtype (type_name (c, L"Writer"));

      SemanticGraph::Type& t (c.type ());

      //@@ need to use FQ-names.
      os << "namespace writer"
         << "{"
         << "void" << std::endl
         << id (name) << " (" << fq_name (t) << " const& s, xercesc::DOMDocument* d)"
         << "{"
         << "xercesc::DOMElement* dom_element = d->getDocumentElement ();"
         << "if (!dom_element)"
         << "{"
         << "throw std::runtime_error (\"XML document has no root element\");"
         << "}"
         << xml_element_type << " e (dom_element);"
         << "if (e.name () != " << string_literal (name) << ")"
         << "{"
         << "throw std::runtime_error (\"unexpected XML root element; expected <"
         << name << ">\");"
         << "}";

      os << "struct W : ";

      WriterBase base (*this);
      base.dispatch (t);

      os << "virtual ::XSCRT::Writer<" << char_type << ">" << std::endl
         << "{"
         << "explicit W (" << xml_element_type << "& e)" << std::endl
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
