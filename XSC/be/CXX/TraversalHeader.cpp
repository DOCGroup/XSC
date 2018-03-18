// file      : CXX/TraversalHeader.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "TraversalHeader.hpp"
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

      if (e.max () != 1)
      {
        // sequence
        //

        // name
        //
        os << "virtual void" << endl
           << id (name) << " (Type&);"
           << endl;

        os << "virtual void" << endl
           << id (name) << " (Type const&);"
           << endl;


        // name_pre
        //
        os << "virtual void" << endl
           << name << "_pre (Type&);"
           << endl;

        os << "virtual void" << endl
           << name << "_pre (Type const&);"
           << endl;


        // name_next
        //
        os << "virtual void" << endl
           << name << "_next (Type&);"
           << endl;

        os << "virtual void" << endl
           << name << "_next (Type const&);"
           << endl;


        // name_post
        //
        os << "virtual void" << endl
           << name << "_post (Type&);"
           << endl;

        os << "virtual void" << endl
           << name << "_post (Type const&);"
           << endl;


        // name_none
        //
        if (e.min () == 0)
        {
          os << "virtual void" << endl
             << name << "_none (Type&);"
             << endl;

          os << "virtual void" << endl
             << name << "_none (Type const&);"
             << endl;
        }
      }
      else if (e.min () == 0)
      {
        // optional
        //

        // name
        //
        os << "virtual void" << endl
           << id (name) << " (Type&);"
           << endl;

        os << "virtual void" << endl
           << id (name) << " (Type const&);"
           << endl;

        // name_none
        //

        os << "virtual void" << endl
           << name << "_none (Type&);"
           << endl;

        os << "virtual void" << endl
           << name << "_none (Type const&);"
           << endl;
      }
      else
      {
        // one
        //

        // name
        //
        os << "virtual void" << endl
           << id (name) << " (Type&);"
           << endl;

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
      string type (type_name (a));

      if (a.optional ())
      {
        // name
        //
        os << "virtual void" << endl
           << id (name) << " (Type&);"
           << endl;

        os << "virtual void" << endl
           << id (name) << " (Type const&);"
           << endl;

        // name_none
        //
        os << "virtual void" << endl
           << name << "_none (Type&);"
           << endl;

        os << "virtual void" << endl
           << name << "_none (Type const&);"
           << endl;
      }
      else
      {
        // name
        //
        os << "virtual void" << endl
           << id (name) << " (Type&);"
           << endl;

        os << "virtual void" << endl
           << id (name) << " (Type const&);"
           << endl;
      }
    }
  };


  struct Base : Traversal::Complex, protected virtual Context
  {
    Base (Context& c)
        : Context (c), member_ (c)
    {
      edge_traverser (inherits_);
      edge_traverser (names_);


      inherits_.node_traverser (*this);
      names_.node_traverser (member_);
    }

    virtual void
    pre (Type& c)
    {
      os << "// From " << type_name (c) << endl;
    }

  private:
    Traversal::Inherits inherits_;

    Member member_;
    Traversal::Names names_;
  };


  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c,
             Traversal::NodeDispatcher& anonymous_type,
             string const& name = L"")
        : Context (c),
          e (c.esymbol.empty () ? c.esymbol : c.esymbol + L" "),
          name_ (name),
          base_ (c),
          member_ (c)
    {
      edge_traverser (inherits_);
      edge_traverser (names_);

      inherits_.node_traverser (base_);
      names_.node_traverser (anonymous_type);
      names_.node_traverser (member_);
    }

    virtual void
    traverse (Type& c)
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
      string override_string (this->cpp11_ ? L" override" : L"");

      os << "struct " << e << name_
         << " : ::XMLSchema::Traversal::Traverser< " << fq_name (c) << " >"
         << "{";

      // traverse ()
      //
      os << "virtual void" << endl
         << "traverse (Type&)" << override_string << ";"
         << endl;

      os << "virtual void" << endl
         << "traverse (Type const&)" << override_string << ";"
         << endl;

      // pre ()
      //
      os << "virtual void" << endl
         << "pre (Type&);"
         << endl;

      os << "virtual void" << endl
         << "pre (Type const&);"
         << endl;
    }

    virtual void
    post (Type& )
    {
      // post ()
      //
      os << "virtual void" << endl
         << "post (Type&);"
         << endl;

      os << "virtual void" << endl
         << "post (Type const&);"
         << endl;

      os << "};";
    }

  private:
    string e;
    string name_;

    Base base_;
    Traversal::Inherits inherits_;

    Member member_;
    Traversal::Names names_;
  };


  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c, string const & name = L"")
        : Context (c),
          name_ (name)
    {
    }

    virtual void
    traverse (Type& e)
    {
      string name ((this->name_ != L"") ? name_: id (e.name ()));
      string type (type_name (e));

      os << "typedef" << endl
         << "::XMLSchema::Traversal::Traverser< " << type << " >" << endl
   << name << ";"
         << endl;
    }

  private:
    string name_;
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

        os << "// anonymous type for " /* << scope << "::"*/ << name << endl;

        if (dynamic_cast<SemanticGraph::Type*> (&e.scope ()))
        {
          os << "public:" << endl;
        }

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
generate_traversal_header (Context& ctx, SemanticGraph::Schema& schema)
{
  ctx.os << "#include \"ace/XML_Utils/XMLSchema/Traversal.hpp\"" << endl
         << endl;

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
  Enumeration enumeration (ctx);

  ns.edge_traverser (names);
  names.node_traverser (complex);
  names.node_traverser (enumeration);
  names.node_traverser (anonymous);

  traverser.traverse (schema);
}
