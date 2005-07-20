// file      : CXX/TypeInfoSource.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <CXX/TypeInfoSource.hpp>
#include <CXX/Elements.hpp>

#include <XSC/SemanticGraph.hpp>
#include <XSC/Traversal.hpp>

namespace
{
  struct Base : Traversal::Type, protected virtual Context
  {
    Base (Context& c)
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
    Complex (Context& c)
        : Context (c), base_ (c)
    {
      edge_traverser (inherits_);
      inherits_.node_traverser (base_);
    }

    virtual void
    pre (Type& c)
    {
      //@@ cache name as a member?
      //
      string name (c.name ());

      os << "struct " << name << "TypeInfoInitializer"
         << "{"
         << name << "TypeInfoInitializer ()"
         << "{"
         << "::XSCRT::TypeId id (typeid (" << id (name) << "));"
         << "::XSCRT::ExtendedTypeInfo nf (id);"
         << endl;
    }

    virtual void
    inherits_none (Type& c)
    {
      string name (c.name ());

      os << "nf.add_base (::XSCRT::ExtendedTypeInfo::Access::public_, "
         << "false, typeid (::XSCRT::Type));";
    }


    virtual void
    post (Type& c)
    {
      string name (c.name ());

      os << "::XSCRT::extended_type_info_map ().insert ("
         << "::std::make_pair (id, nf));"
         << "}"
         << "};"
         << name << "TypeInfoInitializer " << name << "TypeInfoInitializer_;"
         << endl;
    }

  private:
    Base base_;
    Traversal::Inherits inherits_;
  };


  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      string name (e.name ());

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
         << "::std::make_pair (id, nf));"
         << "}"
         << "};"
         << name << "TypeInfoInitializer " << name << "TypeInfoInitializer_;"
         << endl;
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
  ctx.os << "#include \"XMLSchema/TypeInfo.hpp\"" << endl
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
  Complex complex (ctx);
  Enumeration enumeration (ctx);

  ns.edge_traverser (names);
  names.node_traverser (complex);
  names.node_traverser (enumeration);

  traverser.traverse (schema);
}
