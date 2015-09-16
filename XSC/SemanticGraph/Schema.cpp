// file      : XSC/SemanticGraph/Schema.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include <XSC/SemanticGraph/Schema.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    using Introspection::TypeInfo;
    using Introspection::Access;


    // Implies
    //
    //
    namespace
    {
      TypeInfo
      implies_init_ ()
      {
        TypeInfo ti (typeid (Implies));
        ti.add_base (Access::PUBLIC, true, Contains::static_type_info ());
        return ti;
      }

      TypeInfo implies_ (implies_init_ ());
    }

    TypeInfo const& Implies::
    static_type_info () { return implies_; }


    // Sources
    //
    //
    namespace
    {
      TypeInfo
      sources_init_ ()
      {
        TypeInfo ti (typeid (Sources));
        ti.add_base (Access::PUBLIC, true, Contains::static_type_info ());
        return ti;
      }

      TypeInfo sources_ (sources_init_ ());
    }

    TypeInfo const& Sources::
    static_type_info () { return sources_; }


    // Includes
    //
    //
    namespace
    {
      TypeInfo
      includes_init_ ()
      {
        TypeInfo ti (typeid (Includes));
        ti.add_base (Access::PUBLIC, true, Contains::static_type_info ());
        return ti;
      }

      TypeInfo includes_ (includes_init_ ());
    }

    TypeInfo const& Includes::
    static_type_info () { return includes_; }


    // Imports
    //
    //
    namespace
    {
      TypeInfo
      imports_init_ ()
      {
        TypeInfo ti (typeid (Imports));
        ti.add_base (Access::PUBLIC, true, Contains::static_type_info ());
        return ti;
      }

      TypeInfo imports_ (imports_init_ ());
    }

    TypeInfo const& Imports::
    static_type_info () { return imports_; }


    // Schema
    //
    //
    namespace
    {
      TypeInfo
      schema_init_ ()
      {
        TypeInfo ti (typeid (Schema));
        ti.add_base (Access::PUBLIC, true, Container::static_type_info ());
        ti.add_base (Access::PUBLIC, true, Scope::static_type_info ());
        return ti;
      }

      TypeInfo schema_ (schema_init_ ());
    }

    TypeInfo const& Schema::
    static_type_info () { return schema_; }

    Schema::NamesIteratorPair Schema::
    find (Name_ const& name) const
    {
      // Here we are going to create an illusion that
      // the namespace hierarchy is flat.

      names_.clear ();

      // Check our own namespace first so it will end up first in the list.
      //
      NamesIteratorPair pair (Scope::find (name));
      names_.insert (names_.end (), pair.first, pair.second);

      for (ContainsIterator i (contains_begin ()), end (contains_end ());
           i != end; ++i)
      {
        Schema& s (dynamic_cast<Schema&>((**i).element ()));

        pair  = s.find (name);

        names_.insert (names_.end (), pair.first, pair.second);
      }

      return NamesIteratorPair (names_.begin (), names_.end ());
    }
  }
}
