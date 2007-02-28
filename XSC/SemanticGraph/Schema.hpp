// file      : XSC/SemanticGraph/Schema.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef XSC_SEMANTIC_GRAPH_SCHEMA_HXX
#define XSC_SEMANTIC_GRAPH_SCHEMA_HXX

#include <CCF/CompilerElements/FileSystem.hpp>

#include <XSC/SemanticGraph/Elements.hpp>
#include <XSC/SemanticGraph/Namespace.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    class Schema;


    //
    //
    //
    class Implies : public virtual Contains
    {
    public:
      fs::path
      file () const
      {
        return file_;
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Implies (fs::path const& file)
          : file_ (file)
      {
        type_info (static_type_info ());
      }

    private:
      fs::path file_;
    };

    //
    //
    //
    class Sources : public virtual Contains
    {
    public:
      fs::path
      file () const
      {
        return file_;
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Sources (fs::path const& file)
          : file_ (file)
      {
        type_info (static_type_info ());
      }

    private:
      fs::path file_;
    };


    //
    //
    //
    class Includes : public virtual Contains
    {
    public:
      fs::path
      file () const
      {
        return file_;
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Includes (fs::path const& file)
          : file_ (file)
      {
        type_info (static_type_info ());
      }

    private:
      fs::path file_;
    };


    //
    //
    //
    class Imports : public virtual Contains
    {
    public:
      fs::path
      file () const
      {
        return file_;
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Imports (fs::path const& file)
          : file_ (file)
      {
        type_info (static_type_info ());
      }

    private:
      fs::path file_;
    };



    class Schema : public virtual Container,
                   public virtual Scope,
                   public Graph<Node, Edge>
    {
    public:
      Schema ()
          : contained_ (0)
      {
        type_info (static_type_info ());
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    public:

      bool
      contained_p () const
      {
        return contained_ != 0;
      }

      Contains&
      contained () const
      {
        return *contained_;
      }

      virtual NamesIteratorPair
      find (Name_ const& name) const;


    protected:
      friend class Graph<Node, Edge>;

      using Scope::add_edge_left;
      using Container::add_edge_left;

      using Node::add_edge_right;

      void
      add_edge_right (Contains& c)
      {
        contained_ = &c;
      }

    private:
      Schema (Schema const&);
      void operator= (Schema const&);

    private:
      Contains* contained_;

      mutable Names_ names_;

    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_SCHEMA_HXX
