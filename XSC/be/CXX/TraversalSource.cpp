// file      : CXX/TraversalSource.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "TraversalSource.hpp"
#include "Elements.hpp"
#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

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

    void
    optional_member (string const& name, string const& type)
    {
      os << "void " << scope << "::"
         << id (name) << " (Type& o)" << "{";

      if (!this->optional_via_if_set ())
        os << "dispatch (o." << id (name) << " ());";
      else if (this->cppmode_ == CPPMODE::CPP03)
        os << type << " const* value = o." << id (name)
           << "_if_set (); if (value) dispatch (*value);";
      else
        os << "if (auto* value = o." << id (name)
           << "_if_set ()) dispatch (*value);";

      os << "}";

      os << "void " << scope << "::"
         << id (name) << " (Type const& o)" << "{";

      if (!this->optional_via_if_set ())
        os << "dispatch (o." << id (name) << " ());";
      else if (this->cppmode_ == CPPMODE::CPP03 || this->read_only_)
        os << type << " const* value = o." << id (name)
           << "_if_set (); if (value) dispatch (*value);";
      else
        os << "if (auto const* value = o." << id (name)
           << "_if_set ()) dispatch (*value);";

      os << "}";
    }

    void
    traverse (SemanticGraph::Element& e) override
    {
      // Anonymous types cannot be traversed.
      //if (!e.type ().named ())
        //return;

      //@@ rm string scope (id(e.scope ().name ()));
      string name (e.name ());
      string type (type_name (e));

      if (e.minimum () == 0 && e.maximum () == 1)
      {
        // optional

        // name
        optional_member (name, type);

        // name_none
        os << "void " << scope << "::"
           << name << "_none (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::"
           << name << "_none (Type const&)"
           << "{"
           << "}";
      }
      else if (e.minimum () == 1 && e.maximum () == 1)
      {
        // one

        // name
        os << "void " << scope << "::"
           << id (name) << " (Type& o)"
           << "{dispatch (o." << id (name) << " ());}";

        os << "void " << scope << "::"
           << id (name) << " (Type const& o)"
           << "{dispatch (o." << id (name) << " ());}";
      }
      else
      {
        // sequence

        string accessor_name (sequence_accessor_name (e));

        // name
        string iterator_type (fq_name (e, L"", false));
        iterator_type += this->read_only_ ? L"_const_iterator" : L"_iterator";

        os << "void " << scope << "::" << id (name) << " (Type& o)"
           << "{";

        if (this->cppmode_ == CPPMODE::CPP17)
          os << "auto b (o." << accessor_name << " ().begin ()), "
             << "e (o." << accessor_name << " ().end ());";
        else
          os << (this->cppmode_ == CPPMODE::CPP03 ? iterator_type : L"auto") << " "
             << "b (o.begin_" << name << "()), "
             << "e (o.end_" << name << "());";

        os
           << std::endl
           << "if (b != e)"
           << "{"
           << name << "_pre (o);"

           << "for (; b != e;)"
           << "{"
           << (this->cppmode_ == CPPMODE::CPP03 ? "dispatch (*(*b));" : "dispatch (*b);")
           << "if (++b != e) " << name << "_next (o);"
           << "}"

           << name << "_post (o);"
           << "}";

        if (e.minimum () == 0)
          os << "else " << name << "_none (o);";

        os << "}";


        os << "void " << scope << "::" << id (name) << " (Type const& o)"
           << "{";

        if (this->cppmode_ == CPPMODE::CPP17)
          os << "auto b (o." << accessor_name << " ().cbegin ()), "
             << "e (o." << accessor_name << " ().cend ());";
        else
          os << (this->cppmode_ == CPPMODE::CPP03
                   ? fq_name (e, L"", false) + L"_const_iterator"
                   : L"auto") << " "
             << "b (o.begin_" << name << "()), "
             << "e (o.end_" << name << "());";

        os
           << std::endl
           << "if (b != e)"
           << "{"
           << name << "_pre (o);"

           << "for (; b != e;)"
           << "{"
           << (this->cppmode_ == CPPMODE::CPP03 ? "dispatch (*(*b));" : "dispatch (*b);")
           << "if (++b != e) " << name << "_next (o);"
           << "}"

           << name << "_post (o);"
           << "}";

        if (e.minimum () == 0)
          os << "else " << name << "_none (o);";

        os << "}";


        // name_pre
        os << "void " << scope << "::"
           << name << "_pre (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::"
           << name << "_pre (Type const&)"
           << "{"
           << "}";


        // name_next
        os << "void " << scope << "::"
           << name << "_next (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::"
           << name << "_next (Type const&)"
           << "{"
           << "}";


        // name_post
        os << "void " << scope << "::"
           << name << "_post (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::"
           << name << "_post (Type const&)"
           << "{"
           << "}";

        // name_none
        if (e.minimum () == 0)
        {
          os << "void " << scope << "::"
             << name << "_none (Type&)"
             << "{"
             << "}";

          os << "void " << scope << "::"
             << name << "_none (Type const&)"
             << "{"
             << "}";
        }
      }
    }


    void
    traverse (SemanticGraph::Attribute& a) override
    {
      // Anonymous types cannot be traversed.
      //if (!a.type ().named ())
      //  return;

      //@@ rm string scope (id(a.scope ().name ()));
      string name (a.name ());

      if (a.optional ())
      {
        // name
        optional_member (name, type_name (a));

        // name_none
        os << "void " << scope << "::"
           << name << "_none (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::"
           << name << "_none (Type const&)"
           << "{"
           << "}";
      }
      else
      {
        // name
        os << "void " << scope << "::"
           << id (name) << " (Type& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";


        os << "void " << scope << "::"
           << id (name) << " (Type const& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";
      }
    }
  };


  struct Base : Traversal::Complex
  {
    explicit Base (Context& c)
        : member_ (c)
    {
      edge_traverser (inherits_);
      edge_traverser (names_);


      inherits_.node_traverser (*this);
      names_.node_traverser (member_);
    }

  private:
    Traversal::Inherits inherits_;

    Member member_;
    Traversal::Names names_;
  };


  struct CallMember : Traversal::Element,
                      Traversal::Attribute,
                      protected virtual Context
  {
    CallMember (Context& c)
        : Context (c)
    {
    }

    void
    traverse (SemanticGraph::Element& e) override
    {
      // Anonymous types cannot be traversed.
      //if (!e.type ().named ()) return;

      string name (e.name ());

      if (e.minimum () == 0 && e.maximum () == 1)
      {
        // optional
        if (!this->optional_via_if_set ())
          os << "if (o." << name << "_p ()) " << id (name) << " (o);"
             << "else " << name << "_none (o);";
        else
          os << "if (o." << id (name) << "_if_set () != "
             << (this->cppmode_ == CPPMODE::CPP03 ? "0" : "nullptr") << ") "
             << id (name) << " (o);"
             << "else " << name << "_none (o);";
      }
      else if (e.minimum () == 1 && e.maximum () == 1)
      {
        // one
        os << id (name) << " (o);";
      }
      else
      {
        // sequence
        os << id (name) << " (o);";
      }
    }

    void
    traverse (SemanticGraph::Attribute& a) override
    {
      // Anonymous types cannot be traversed.
      //if (!a.type ().named ()) return;

      string name (a.name ());

      if (a.optional ())
      {
        // optional
        if (!this->optional_via_if_set ())
          os << "if (o." << name << "_p ()) " << id (name) << " (o);"
             << "else " << name << "_none (o);";
        else
          os << "if (o." << id (name) << "_if_set () != "
             << (this->cppmode_ == CPPMODE::CPP03 ? "0" : "nullptr") << ") "
             << id (name) << " (o);"
             << "else " << name << "_none (o);";
      }
      else
      {
        // one
        os << id (name) << " (o);";
      }
    }
  };


  struct CallBase : Traversal::Complex
  {
    explicit CallBase (Context& c)
        : member_ (c)
    {
      edge_traverser (inherits_);
      edge_traverser (names_);

      inherits_.node_traverser (*this);
      names_.node_traverser (member_);
    }

  private:
    Traversal::Inherits inherits_;

    CallMember member_;
    Traversal::Names names_;
  };


  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c,
             Traversal::NodeDispatcher& anonymous_type,
             string const& name = L"")
        : Context (c),
          name_ (name),
          base_ (c),
          member_ (c),
          call_base_ (c),
          call_member_ (c)
    {
      edge_traverser (inherits_);
      edge_traverser (names_);

      inherits_.node_traverser (base_);
      names_.node_traverser (member_);
      names_anonymous_.node_traverser (anonymous_type);

      call_inherits_.node_traverser (call_base_);
      call_names_.node_traverser (call_member_);
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
      //string name (id (name_));

      //enter_scope (name);

      // traverse (Type&)
      os << "void " << scope << "::"
         << "traverse (Type& o)"
         << "{"
         << "pre (o);";

      inherits (c, call_inherits_);
      names (c, call_names_);

      os << "post (o);"
         << "}";

      // traverse (Type const&)
      os << "void " << scope << "::"
         << "traverse (Type const& o)"
         << "{"
         << "pre (o);";

      inherits (c, call_inherits_);
      names (c, call_names_);

      os << "post (o);"
         << "}";


      // pre ()
      os << "void " << scope << "::"
         << "pre (Type&)"
         << "{"
         << "}";

      os << "void " << scope << "::"
         << "pre (Type const&)"
         << "{"
         << "}";
    }

    void
    post (Type&) override
    {
      // string name (id (name_));

      // post ()
      os << "void " << scope << "::"
         << "post (Type&)"
         << "{"
         << "}";

      os << "void " << scope << "::"
         << "post (Type const&)"
         << "{"
         << "}";

      //leave_scope ();
    }

  private:
    string name_;

    Base base_;
    Traversal::Inherits inherits_;

    Member member_;
    Traversal::Names names_;

    CallBase call_base_;
    Traversal::Inherits call_inherits_;

    CallMember call_member_;
    Traversal::Names call_names_;

    Traversal::Names names_anonymous_;
  };

  struct TraversalNamespace : Namespace
  {
    TraversalNamespace (Context& c)
        : Context (c), Namespace (c)
    {
    }

    void
    pre (Type& n) override
    {
      Namespace::pre (n);

      os << "namespace Traversal"
         << "{";

    }

    void
    post (Type& n) override
    {
      os << "}";

      Namespace::post (n);
    }
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

        belongs.node_traverser (complex_element);

        t.context ().set ("seen", true);

        Element::belongs (e, belongs);

        t.context ().remove ("seen");
      }
    }
  };
}

void
generate_traversal_source (Context& ctx, SemanticGraph::Schema& schema)
{
  Traversal::Schema traverser;
  Traversal::Sources sources;
  Traversal::Names schema_names;
  TraversalNamespace ns (ctx);

  traverser.edge_traverser (sources);
  traverser.edge_traverser (schema_names);
  sources.node_traverser (traverser);
  schema_names.node_traverser (ns);

  Traversal::Names names;
  AnonymousType anonymous (ctx);
  Complex complex (ctx, anonymous);
  Traversal::Enumeration enumeration;

  ns.edge_traverser (names);
  names.node_traverser (complex);
  names.node_traverser (enumeration);
  names.node_traverser (anonymous);

  traverser.traverse (schema);
}
