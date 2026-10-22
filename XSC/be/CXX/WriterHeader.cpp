// file      : CXX/WriterHeader.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

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
    explicit Member (Context& c)
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
        if (this->cppmode_ == CPPMODE::CPP03)
        {
          // Borland post
          os << "virtual void" << std::endl
            << id (name)  << " (Type &o)" << std::endl
            << "{"
            << "this->" << id (name) << " ("
            << "const_cast <Type const &> (o));" << std::endl
            << "}";
        }

        // optional
        //
        os << "virtual void" << std::endl
           << id (name) << " (Type const&);"
           << std::endl;
      }
      else if (e.min () == 1 && e.max () == 1)
      {
        if (this->cppmode_ == CPPMODE::CPP03)
        {
          // Borland post
          os << "virtual void" << std::endl
            << id (name)  << " (Type &o)" << std::endl
            << "{"
            << "this->" << id (name) << " ("
            << "const_cast <Type const &> (o));" << std::endl
            << "}";
        }

        // one
        //
        os << "virtual void" << std::endl
           << id (name) << " (Type const&);"
           << std::endl;
      }
      else
      {
        // sequence
        //

        // Pre
        //
        if (this->cppmode_ == CPPMODE::CPP03)
        {
          os << "virtual void" << std::endl
            << name << "_pre (Type &o)" << std::endl
            << "{"
            << "this->"  << name << "_pre ("
            << "const_cast <Type const &> (o));" << std::endl
            << "}";
        }

        // pre
        //
        os << "virtual void" << std::endl
           << name << "_pre (Type const&);"
           << std::endl;

        // non-const next
        //
        if (this->cppmode_ == CPPMODE::CPP03)
        {
          os << "virtual void" << std::endl
            << name << "_next (Type &o)" << std::endl
            << "{"
            << "this->"  << name << "_next ("
            << "const_cast <Type const &> (o));" << std::endl
            << "}";
        }

        // next
        //
        os << "virtual void" << std::endl
           << name << "_next (Type const&);"
           << std::endl;

        //  post
        if (this->cppmode_ == CPPMODE::CPP03)
        {
          os << "virtual void" << std::endl
            << name << "_post (Type &o)" << std::endl
            << "{"
            << "this->" << name << "_post ("
            << "const_cast <Type const &> (o));" << std::endl
            << "}";
        }

        // post
        //
        os << "virtual void" << std::endl
           << name << "_post (Type const&);"
           << std::endl;
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
      if (this->cppmode_ == CPPMODE::CPP03)
      {
        os << "virtual void" << std::endl
          << id (name)  << " (Type &o)" << std::endl
          << "{"
          << "this->" << id (name) << " ("
          << "const_cast <Type const &> (o));" << std::endl
          << "}";
      }

      os << "virtual void" << std::endl
         << id (name) << " (Type const&);"
         << std::endl;
    }
  };

  struct Base : // Traversal::Type,
                Fundamental,
                protected virtual Context
  {
    explicit Base (Context& c)
        : Context (c)
    {
    }

    //virtual void
    //traverse (SemanticGraph::Type& t)
    //{
    //  string type (type_name (t, L"Writer"));

    //  os << "virtual " << type << "," << std::endl;
    //}

    virtual void
    traverse (SemanticGraph::IdRef&)
    {
      os << "virtual ::XMLSchema::Writer::IDREF<" << char_type << ">, " <<
        std::endl;
    }

    virtual void
    fundamental_type (SemanticGraph::FundamentalType & t)
    {
      string name (id (t.name ()));

      os << "virtual ::XMLSchema::Writer::FundamentalType< " <<
        "::XMLSchema::" << name << ", " << char_type << " >," << std::endl;
    }

    void
    fundamental_template (SemanticGraph::FundamentalType& t)
    {
      string name (id (t.name ()));

      os << "virtual ::XMLSchema::Writer::FundamentalType< " <<
        "::XMLSchema::" << name << "< " << char_type << " >, " <<
        char_type << " >," << std::endl;
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
      os << "struct " << name_ << " : Traversal::" << scope << "," << std::endl;

      inherits (c, inherits_);

      os << "virtual ::XSCRT::Writer<" << char_type << ">"
         << "{";

      //@@ Should probably be Type__
      //
      if (this->cppmode_ != CPPMODE::CPP03)
      {
        os << "using Type =" << fq_name (c) << ";";
      }
      else
      {
        os << "typedef " << fq_name (c) << " Type;";
      }

      // c-tor
      //
      os << "explicit " << name_ << " (" << xml_element_type << "&);"
         << std::endl;

      // Non-const traverse for Borland
      if (this->cppmode_ == CPPMODE::CPP03)
      {
        os << "virtual void" << std::endl
          << "traverse (Type &o)" << std::endl
          << "{"
          << "this->traverse ("
          << "const_cast <Type const &> (o));"
          << "}";
      }

      string virtual_string (this->cppmode_ == CPPMODE::CPP03 ? L"virtual " : L"");
      string override_string (this->cppmode_ != CPPMODE::CPP03 ? L" override" : L"");

      // traverse
      //
      os << virtual_string << "void" << std::endl
         << "traverse (Type const&)" << override_string << ";"
         << std::endl;
    }

    virtual void
    post (Type& )
    {
      string default_string (this->cppmode_ != CPPMODE::CPP03 ? L" = default" : L"");
      os << "protected:" << std::endl
         << name_ << " ()" << default_string << ";"
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
      string virtual_string (this->cppmode_ == CPPMODE::CPP03 ? L"virtual " : L"");
      string override_string (this->cppmode_ != CPPMODE::CPP03 ? L" override" : L"");
      string default_string (this->cppmode_ != CPPMODE::CPP03 ? L" = default" : L"");

      if (name == L"") name = L"BAD NAME";

      os << "struct " << name << " : Traversal::" << name << "," << std::endl
         << "virtual ::XSCRT::Writer<" << char_type << ">"
         << "{";

      // c-tor
      //
      os << "explicit " << name << " (" << xml_element_type << "&);"
         << std::endl;

      // Non-const traverse for Borland
      if (this->cppmode_ == CPPMODE::CPP03)
      {
        os << "virtual void" << std::endl
          << "traverse (Type &o)"
          << "{"
          << "this->traverse ("
          << "const_cast <Type const &> (o));"
          << "}";
      }

      // traverse
      //
      os << virtual_string << "void" << std::endl
         << "traverse (Type const&)" << override_string << ";"
         << std::endl;

      os << "protected:" << std::endl
         << name << " ()" << default_string << ";"
         << "};";
    }

  private:
    string ex;

    string name_;
  };


  struct WriterNamespace : Namespace
  {
    explicit WriterNamespace (Context& c)
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
         << "void" << std::endl
         << id (name) << " (" << type << " const&, xercesc::DOMDocument*);"
         << "}"
         << std::endl;
    }

  private:
    string e;
  };

  struct AnonymousType : Traversal::Element, protected virtual Context
  {
    explicit AnonymousType (Context& c)
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

        os << "// anonymous type for " /* << scope << "::"*/ << name << std::endl;

        if (dynamic_cast<SemanticGraph::Type*> (&e.scope ()))
          os << "public:" << std::endl;

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
  ctx.os << "#include \"ace/XML_Utils/XMLSchema/Writer.hpp\"" << std::endl
         << std::endl;

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
