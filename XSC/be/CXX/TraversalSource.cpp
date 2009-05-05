// file      : CXX/TraversalSource.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "TraversalSource.hpp"
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
        //return;

      //@@ rm string scope (id(e.scope ().name ()));
      string name (e.name ());
      string type (type_name (e));

      if (e.min () == 0 && e.max () == 1)
      {
        // optional
        //

        // name
        //
        os << "void " << scope << "::" << endl
           << id (name) << " (Type& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";

        os << "void " << scope << "::" << endl
           << id (name) << " (Type const& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";

        // name_none
        //
        os << "void " << scope << "::" << endl
           << name << "_none (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::" << endl
           << name << "_none (Type const&)"
           << "{"
           << "}";
      }
      else if (e.min () == 1 && e.max () == 1)
      {
        // one
        //

        // name
        //
        os << "void " << scope << "::" << endl
           << id (name) << " (Type& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";

        os << "void " << scope << "::" << endl
           << id (name) << " (Type const& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";
      }
      else
      {
        // sequence
        //

        // name
        //
        os << "void " << scope << "::" << endl
           << id (name) << " (Type& o)"
           << "{"
           << "// VC6 anathema strikes again" << endl
           << "//" << endl
           << fq_name (e, L"", false) << "_iterator "
           << "b (o.begin_" << name << "()), "
           << "e (o.end_" << name << "());"
           << endl
           << "if (b != e)"
           << "{"
           << name << "_pre (o);"

          //@@ VC6
          //
          // << "iterate_and_dispatch< " << scope << ", Type > " <<
          //"(b, e, *this, &" << scope << "::" << name << "_next, o);"

           << "for (; b != e;)"
           << "{"
           << "dispatch (*(*b));"
           << "if (++b != e) " << name << "_next (o);"
           << "}"

           << name << "_post (o);"
           << "}";

        if (e.min () == 0)
          os << "else " << name << "_none (o);";

        os << "}";


        os << "void " << scope << "::" << endl
           << id (name) << " (Type const& o)"
           << "{"
           << "// VC6 anathema strikes again" << endl
           << "//" << endl
           << fq_name (e, L"", false) << "_const_iterator "
           << "b (o.begin_" << name << "()), "
           << "e (o.end_" << name << "());"
           << endl
           << "if (b != e)"
           << "{"
           << name << "_pre (o);"

          //@@ VC6
          //
          // << "iterate_and_dispatch< " << scope << ", Type const > " <<
          //"(b, e, *this, &" << scope << "::" << name << "_next, o);"

           << "for (; b != e;)"
           << "{"
           << "dispatch (*(*b));"
           << "if (++b != e) " << name << "_next (o);"
           << "}"

           << name << "_post (o);"
           << "}";

        if (e.min () == 0)
          os << "else " << name << "_none (o);";

        os << "}";


        // name_pre
        //
        os << "void " << scope << "::" << endl
           << name << "_pre (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::" << endl
           << name << "_pre (Type const&)"
           << "{"
           << "}";


        // name_next
        //
        os << "void " << scope << "::" << endl
           << name << "_next (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::" << endl
           << name << "_next (Type const&)"
           << "{"
           << "}";


        // name_post
        //
        os << "void " << scope << "::" << endl
           << name << "_post (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::" << endl
           << name << "_post (Type const&)"
           << "{"
           << "}";

        // name_none
        //
        if (e.min () == 0)
        {
          os << "void " << scope << "::" << endl
             << name << "_none (Type&)"
             << "{"
             << "}";

          os << "void " << scope << "::" << endl
             << name << "_none (Type const&)"
             << "{"
             << "}";
        }
      }
    }


    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      // Anonymous types cannot be traversed.
      //
      //if (!a.type ().named ())
      //  return;

      //@@ rm string scope (id(a.scope ().name ()));
      string name (a.name ());
      string type (type_name (a));

      if (a.optional ())
      {
        // name
        //
        os << "void " << scope << "::" << endl
           << id (name) << " (Type& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";

        os << "void " << scope << "::" << endl
           << id (name) << " (Type const& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";

        // name_none
        //
        os << "void " << scope << "::" << endl
           << name << "_none (Type&)"
           << "{"
           << "}";

        os << "void " << scope << "::" << endl
           << name << "_none (Type const&)"
           << "{"
           << "}";
      }
      else
      {
        // name
        //
        os << "void " << scope << "::" << endl
           << id (name) << " (Type& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";


        os << "void " << scope << "::" << endl
           << id (name) << " (Type const& o)"
           << "{"
           << "dispatch (o." << id (name) << " ());"
           << "}";
      }
    }
  };


  struct Base : Traversal::Complex
  {
    Base (Context& c)
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

    virtual void
    traverse (SemanticGraph::Element& e)
    {
      // Anonymous types cannot be traversed.
      //
      //if (!e.type ().named ()) return;

      string name (e.name ());

      if (e.min () == 0 && e.max () == 1)
      {
        // optional
        //
        os << "if (o." << name << "_p ()) " << id (name) << " (o);"
           << "else " << name << "_none (o);";
      }
      else if (e.min () == 1 && e.max () == 1)
      {
        // one
        //
        os << id (name) << " (o);";
      }
      else
      {
        // sequence
        //
        os << id (name) << " (o);";
      }
    }

    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      // Anonymous types cannot be traversed.
      //
      //if (!a.type ().named ()) return;

      string name (a.name ());

      if (a.optional ())
      {
        // optional
        //
        os << "if (o." << name << "_p ()) " << id (name) << " (o);"
           << "else " << name << "_none (o);";
      }
      else
      {
        // one
        //
        os << id (name) << " (o);";
      }
    }
  };


  struct CallBase : Traversal::Complex
  {
    CallBase (Context& c)
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
      //string name (id (name_));

      //enter_scope (name);

      os << "// " << scope << endl
         << "//" << endl
         << "//" << endl
         << endl;

      // traverse (Type&)
      //
      os << "void " << scope << "::" << endl
         << "traverse (Type& o)"
         << "{"
         << "pre (o);";

      inherits (c, call_inherits_);
      names (c, call_names_);

      os << "post (o);"
         << "}";

      // traverse (Type const&)
      //
      os << "void " << scope << "::" << endl
         << "traverse (Type const& o)"
         << "{"
         << "pre (o);";

      inherits (c, call_inherits_);
      names (c, call_names_);

      os << "post (o);"
         << "}";


      // pre ()
      //
      os << "void " << scope << "::" << endl
         << "pre (Type&)"
         << "{"
         << "}";

      os << "void " << scope << "::" << endl
         << "pre (Type const&)"
         << "{"
         << "}";
    }

    virtual void
    post (Type& c)
    {
      // string name (id (name_));

      // post ()
      //
      os << "void " << scope << "::" << endl
         << "post (Type&)"
         << "{"
         << "}";

      os << "void " << scope << "::" << endl
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

    virtual void
    pre (Type& n)
    {
      Namespace::pre (n);

      os << "namespace Traversal"
         << "{";

    }

    virtual void
    post (Type& n)
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

    virtual void
    traverse (Type& e)
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
