// file      : XSC/SemanticGraph/Elements.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef XSC_SEMANTIC_GRAPH_ELEMENTS_HPP
#define XSC_SEMANTIC_GRAPH_ELEMENTS_HPP

#include <string>

#include <XSC/SemanticGraph/Graph.hpp>
#include <XSC/SemanticGraph/Context.hpp>

#include <Utility/Introspection/Introspection.hpp>


namespace Introspection = Utility::Introspection;

namespace XSC
{
  namespace SemanticGraph
  {
    //
    //
    //
    class Node;
    class Edge;


    // Node & Edge
    //
    //
    class Edge : public virtual Introspection::Object
    {
    public:
      Context const&
      context () const
      {
        return context_;
      }

      Context&
      context ()
      {
        return context_;
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      virtual
      ~Edge ()
      {
      }


      Edge ()
      {
        type_info (static_type_info ());
      }

    private:
      Context context_;
    };

    inline bool
    operator== (Edge const& x, Edge const& y)
    {
      return &x == &y;
    }


    class Node : public virtual Introspection::Object
    {
    public:
      Context const&
      context () const
      {
        return context_;
      }

      Context&
      context ()
      {
        return context_;
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      virtual
      ~Node ()
      {
      }

      Node ()
      {
        type_info (static_type_info ());
      }


      // This is a bunch of experimental sink functions that allow
      // extensions in the form of one-way edges (see Executor stuff
      // in CIDL for example).
      //
      //void
      //add_edge_left (Edge&)
      //{
      //}

      void
      add_edge_right (Edge&)
      {
      }

    private:
      Context context_;
    };

    inline bool
    operator== (Node const& x, Node const& y)
    {
      return &x == &y;
    }

    //
    //
    //
    typedef
    std::wstring
    Name_;

    //
    //
    //
    class Scope;
    class Nameable;

    class Names : public virtual Edge
    {
    public:
      Name_
      name () const
      {
        return name_;
      }

      Scope&
      scope () const
      {
        return *scope_;
      }

      Nameable&
      named () const
      {
        return *named_;
      }

      bool
      anonymous () const
      {
        return anonymous_;
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Names (Name_ const& name, bool anonymous = false)
          : name_ (name),
            anonymous_ (anonymous)
      {
        type_info (static_type_info ());
      }

      void
      set_left_node (Scope& n)
      {
        scope_ = &n;
      }

      void
      set_right_node (Nameable& n)
      {
        named_ = &n;
      }

    private:
      Scope* scope_;
      Nameable* named_;
      Name_ name_;
      bool anonymous_;
    };


    class Nameable : public virtual Node
    {
      typedef
      std::vector<Names*>
      Named_;

    public:
      bool
      named () const
      {
        return !named_[0]->anonymous ();
      }

      Name_
      name () const
      {
        return named_[0]->name ();
      }

      Scope&
      scope ()
      {
        return named_[0]->scope ();
      }

      /*
      typedef
      Named_::const_iterator
      NamedIterator;

      NamedIterator
      named_begin () const
      {
        return named_.begin ();
      }

      NamedIterator
      named_end () const
      {
        return named_.end ();
      }
      */

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Nameable ()
      {
        type_info (static_type_info ());
      }

      void
      add_edge_right (Names& e)
      {
        named_.push_back (&e);
      }

    private:
      Named_ named_;
    };

    typedef
    std::set<Nameable*>
    Nameables;


    //
    //
    //
    class Scope : public virtual Nameable
    {
    protected:
      typedef
      std::vector<Names*>
      Names_;

      typedef
      std::map<Name_, Names_>
      NamesMap_;

    public:
      //
      //
      typedef
      Names_::const_iterator
      NamesIterator;

      typedef
      std::pair <NamesIterator, NamesIterator>
      NamesIteratorPair;

      NamesIterator
      names_begin () const
      {
        return names_.begin ();
      }

      NamesIterator
      names_end () const
      {
        return names_.end ();
      }

      virtual NamesIteratorPair
      find (Name_ const& name) const
      {
        NamesMap_::const_iterator i (names_map_.find (name));

        if (i == names_map_.end ())
        {
          return NamesIteratorPair (names_.end (), names_.end ());
        }
        else
        {
          return NamesIteratorPair (i->second.begin (), i->second.end ());
        }
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Scope ()
      {
        type_info (static_type_info ());
      }

      void
      add_edge_left (Names& e)
      {
        names_.push_back (&e);
        names_map_[e.name ()].push_back (&e);
      }

      using Nameable::add_edge_right;

    private:
      Names_ names_;
      NamesMap_ names_map_;
    };



    //
    //
    //
    class Belongs;

    class Type : public virtual Nameable
    {
    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Type ()
      {
        type_info (static_type_info ());
      }

      using Nameable::add_edge_right;

      void
      add_edge_right (Belongs& e)
      {
        slassifies_.push_back (&e);
      }

      using Node::add_edge_right;
      //using Node::add_edge_left;

    private:
      typedef
      std::vector<Belongs*>
      Classifies_;

      Classifies_ slassifies_;
    };


    class Instance : public virtual Nameable
    {
    public:
      Belongs&
      belongs () const
      {
        return *belongs_;
      }

      Type&
      type () const;

      bool
      typed () const
      {
        return belongs_ != 0;
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Instance ()
          : belongs_ (0)
      {
        type_info (static_type_info ());
      }

      void
      add_edge_left (Belongs& e)
      {
        belongs_ = &e;
      }

    private:
      Belongs* belongs_;
    };


    class Belongs : public virtual Edge
    {
    public:
      Instance&
      instance () const
      {
        return *instance_;
      }

      Type&
      type () const
      {
        return *type_;
      }


      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Belongs ()
      {
        type_info (static_type_info ());
      }

      void
      set_left_node (Instance& n)
      {
        instance_ = &n;
      }

      void
      set_right_node (Type& n)
      {
        type_ = &n;
      }

    private:
      Instance* instance_;
      Type* type_;
    };


    //
    //
    //
    class Inherits : public virtual Edge
    {
    public:
      virtual Type&
      inheritor () const
      {
        return *inheritor_;
      }

      virtual Type&
      inheritee () const
      {
        return *inheritee_;
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Inherits ()
      {
        type_info (static_type_info ());
      }

      void
      set_left_node (Type& n)
      {
        inheritor_ = &n;
      }

      void
      set_right_node (Type& n)
      {
        inheritee_ = &n;
      }

    private:
      Type* inheritor_;
      Type* inheritee_;
    };


    //
    //
    //
    class Container;

    class Contains : public virtual Edge
    {
    public:
      Node&
      element () const
      {
        return *element_;
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Contains ()
      {
        type_info (static_type_info ());
      }

      void
      set_left_node (Container& n)
      {
        container_ = &n;
      }

      void
      set_right_node (Node& n)
      {
        element_ = &n;
      }

    private:
      Container* container_;
      Node* element_;
    };


    class Container : public virtual Node
    {
      typedef
      std::vector<Contains*>
      Contains_;

    public:
      typedef
      Contains_::const_iterator
      ContainsIterator;

      ContainsIterator
      contains_begin () const
      {
        return contains_.begin ();
      }

      ContainsIterator
      contains_end () const
      {
        return contains_.end ();
      }

      static Introspection::TypeInfo const&
      static_type_info ();

    protected:
      friend class Graph<Node, Edge>;

      Container ()
      {
        type_info (static_type_info ());
      }

      void
      add_edge_left (Contains& e)
      {
        contains_.push_back (&e);
      }

    private:
      Contains_ contains_;
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_ELEMENTS_HPP
