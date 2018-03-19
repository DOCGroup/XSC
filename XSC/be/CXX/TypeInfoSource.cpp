// file      : CXX/TypeInfoSource.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "TypeInfoSource.hpp"
#include "Elements.hpp"

#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

namespace
{
  struct Base : Traversal::Type, protected virtual Context
  {
    explicit Base (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (SemanticGraph::Type& c)
    {
      string name (type_name (c));

      os << "nf.add_base (::XSCRT::ExtendedTypeInfo::Access::public_, "
         << "false, typeid (" << name << "));";
    }
  };


  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c,
             Traversal::NodeDispatcher& anonymous_type,
             string const& name = L"")
        : Context (c),
          name_ (name),
          base_ (c)
    {
      edge_traverser (inherits_);

      inherits_.node_traverser (base_);

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

        Traversal::Complex::traverse (c);

        c.context ().remove ("name");

        leave_scope ();
      }
    }

    virtual void
    pre (Type& c)
    {
      //@@ cache name as a member?
      //

      os << "struct " << name_ << "TypeInfoInitializer"
         << "{"
         << name_ << "TypeInfoInitializer ()"
         << "{"
         << "::XSCRT::TypeId id (typeid (" << fq_name (c) << "));"
         << "::XSCRT::ExtendedTypeInfo nf (id);"
         << endl;
    }

    virtual void
    inherits_none (Type& )
    {
      os << "nf.add_base (::XSCRT::ExtendedTypeInfo::Access::public_, "
         << "false, typeid (::XSCRT::Type));";
    }


    virtual void
    post (Type& c)
    {
      os << "::XSCRT::extended_type_info_map ().insert ("
         << "std::make_pair (id, nf));"
         << "}";

      // Go after anonymous types.
      //
      names (c, names_anonymous_);

      os << "};"
         << name_ << "TypeInfoInitializer " << name_ << "TypeInfoInitializer_;"
         << endl;
    }

  private:
    string name_;

    Base base_;

    Traversal::Inherits inherits_;

    Traversal::Names names_anonymous_;
  };


  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c,
                 string const & name = L"")
    : Context (c),
      name_ (name)
    {
    }

    virtual void
    traverse (Type& e)
    {
      string name ((this->name_ != L"") ? name_ : id (e.name ()));

      os << "struct " << name << "TypeInfoInitializer"
         << "{"
         << name << "TypeInfoInitializer ()"
         << "{"
         << "::XSCRT::TypeId id (typeid (" << id (name) << "));"
         << "::XSCRT::ExtendedTypeInfo nf (id);"
         << endl
         << "nf.add_base (::XSCRT::ExtendedTypeInfo::Access::public_, "
         << "false, typeid (::XSCRT::Type));"
         << endl
         << "::XSCRT::extended_type_info_map ().insert ("
         << "std::make_pair (id, nf));"
         << "}"
         << "};"
         << name << "TypeInfoInitializer " << name << "TypeInfoInitializer_;"
         << endl;
    }

  private:
    string name_;
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

        belongs.node_traverser (complex);

        t.context ().set ("seen", true);

        Element::belongs (e, belongs);

        t.context ().remove ("seen");
      }
    }
  };

  struct TypeInfoNamespace : Namespace
  {
    TypeInfoNamespace (Context& c)
        : Context (c), Namespace (c)
    {
    }

    virtual void
    pre (Type& n)
    {
      Namespace::pre (n);

      os << "namespace"
         << "{"
         << "::XMLSchema::TypeInfoInitializer < " << char_type << " > "
         << "XMLSchemaTypeInfoInitializer_ (::XSCRT::extended_type_info_map ());"
         << endl;

    }

    virtual void
    post (Type& n)
    {
      os << "}";

      Namespace::post (n);
    }
  };

}

void
generate_type_info_source (Context& ctx, SemanticGraph::Schema& schema)
{
  ctx.os << "#include \"ace/XML_Utils/XMLSchema/TypeInfo.hpp\"" << endl
         << endl;

  Traversal::Schema traverser;
  Traversal::Sources sources;
  Traversal::Names schema_names;
  TypeInfoNamespace ns (ctx);

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
