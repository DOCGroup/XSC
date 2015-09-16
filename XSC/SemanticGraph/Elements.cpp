// file      : XSC/SemanticGraph/Elements.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include <XSC/SemanticGraph/Elements.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    using Introspection::TypeInfo;
    using Introspection::Access;

    // Edge
    //
    //
    namespace
    {
      TypeInfo
      edge_init_ ()
      {
        TypeInfo ti (typeid (Edge));
        // I don't really need this information
        // ti.add_base (Access::PUBLIC, true, Object::static_type_info ());
        return ti;
      }

      TypeInfo edge_ (edge_init_ ());
    }

    TypeInfo const& Edge::
    static_type_info () { return edge_; }


    // Node
    //
    //
    namespace
    {
      TypeInfo
      node_init_ ()
      {
        TypeInfo ti (typeid (Node));
        // I don't really need this information
        // ti.add_base (Access::PUBLIC, true, Object::static_type_info ());
        return ti;
      }

      TypeInfo node_ (node_init_ ());
    }

    TypeInfo const& Node::
    static_type_info () { return node_; }


    // Names
    //
    //
    namespace
    {
      TypeInfo
      names_init_ ()
      {
        TypeInfo ti (typeid (Names));
        ti.add_base (Access::PUBLIC, true, Edge::static_type_info ());
        return ti;
      }

      TypeInfo names_ (names_init_ ());
    }

    TypeInfo const& Names::
    static_type_info () { return names_; }

    // Nameable
    //
    //
    namespace
    {
      TypeInfo
      nameable_init_ ()
      {
        TypeInfo ti (typeid (Nameable));
        ti.add_base (Access::PUBLIC, true, Node::static_type_info ());
        return ti;
      }

      TypeInfo nameable_ (nameable_init_ ());
    }

    TypeInfo const& Nameable::
    static_type_info () { return nameable_; }



    // Scope
    //
    //
    namespace
    {
      TypeInfo
      scope_init_ ()
      {
        TypeInfo ti (typeid (Scope));
        ti.add_base (Access::PUBLIC, true, Nameable::static_type_info ());
        return ti;
      }

      TypeInfo scope_ (scope_init_ ());
    }

    TypeInfo const& Scope::
    static_type_info () { return scope_; }


    // Type
    //
    //
    namespace
    {
      TypeInfo
      type_init_ ()
      {
        TypeInfo ti (typeid (Type));
        ti.add_base (Access::PUBLIC, true, Nameable::static_type_info ());
        return ti;
      }

      TypeInfo type_ (type_init_ ());
    }

    TypeInfo const& Type::
    static_type_info () { return type_; }


    // Instance
    //
    //

    Type& Instance::
    type () const
    {
      return belongs ().type ();
    }

    namespace
    {
      TypeInfo
      instance_init_ ()
      {
        TypeInfo ti (typeid (Instance));
        ti.add_base (Access::PUBLIC, true, Nameable::static_type_info ());
        return ti;
      }

      TypeInfo instance_ (instance_init_ ());
    }

    TypeInfo const& Instance::
    static_type_info () { return instance_; }


    // Belongs
    //
    //
    namespace
    {
      TypeInfo
      belongs_init_ ()
      {
        TypeInfo ti (typeid (Belongs));
        ti.add_base (Access::PUBLIC, true, Edge::static_type_info ());
        return ti;
      }

      TypeInfo belongs_ (belongs_init_ ());
    }

    TypeInfo const& Belongs::
    static_type_info () { return belongs_; }


    // Inherits
    //
    //
    namespace
    {
      TypeInfo
      inherits_init_ ()
      {
        TypeInfo ti (typeid (Inherits));
        ti.add_base (Access::PUBLIC, true, Node::static_type_info ());
        return ti;
      }

      TypeInfo inherits_ (inherits_init_ ());
    }

    TypeInfo const& Inherits::
    static_type_info () { return inherits_; }


    // Contains
    //
    //
    namespace
    {
      TypeInfo
      contains_init_ ()
      {
        TypeInfo ti (typeid (Contains));
        ti.add_base (Access::PUBLIC, true, Edge::static_type_info ());
        return ti;
      }

      TypeInfo contains_ (contains_init_ ());
    }

    TypeInfo const& Contains::
    static_type_info () { return contains_; }


    // Container
    //
    //
    namespace
    {
      TypeInfo
      container_init_ ()
      {
        TypeInfo ti (typeid (Container));
        ti.add_base (Access::PUBLIC, true, Node::static_type_info ());
        return ti;
      }

      TypeInfo container_ (container_init_ ());
    }

    TypeInfo const& Container::
    static_type_info () { return container_; }
  }
}
