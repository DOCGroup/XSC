// file      : XSC/Traversal/Elements.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_TRAVERSAL_ELEMENTS_HPP
#define XSC_TRAVERSAL_ELEMENTS_HPP

#include <map>
#include <set>

#include "Utility/Introspection/Introspection.hpp"

#include "XSC/SemanticGraph/Elements.hpp"

namespace Introspection = Utility::Introspection;

/*
#include <iostream>

using std::cerr;
using std::endl;
*/

namespace XSC
{
  namespace Traversal
  {
    //
    //
    //
    template<typename T>
    class Traverser
    {
    protected:
      virtual
      ~Traverser ();

      virtual void
      trampoline (T& n) = 0;

      template <typename>
      friend class Dispatcher;
    };


    //
    //
    //
    template <typename T>
    class Dispatcher
    {
    public:
      virtual
      ~Dispatcher ();

      virtual void
      dispatch (T& n);

      void
      map (Introspection::TypeId id, Traverser<T>& t)
      {
        Traversers& traversers (traversal_map_[id]);
        traversers.push_back (&t);
      }

    public:
      typedef
      std::vector<Traverser<T>*>
      Traversers;

      typedef
      std::map<Introspection::TypeId, Traversers>
      TraversalMap;

      typedef
      typename TraversalMap::const_iterator
      Iterator;

      Iterator
      begin () const
      {
        return traversal_map_.begin ();
      }

      Iterator
      end () const
      {
        return traversal_map_.end ();
      }

    private:
      struct TypeInfoComparator
      {
        bool
        operator () (Introspection::TypeInfo const& x,
                     Introspection::TypeInfo const& y) const
        {
          return x.type_id () < y.type_id ();
        }
      };

      typedef
      std::map<Introspection::TypeInfo, unsigned long, TypeInfoComparator>
      LevelMap;

      typedef
      std::set<Introspection::TypeInfo, TypeInfoComparator>
      TypeInfoSet;

      static unsigned long
      compute_levels (Introspection::TypeInfo const& ti,
                      unsigned long cur,
                      LevelMap& map);

      static void
      flatten_tree (Introspection::TypeInfo const& ti, TypeInfoSet& set);

    private:
      TraversalMap traversal_map_;
    };


    //
    //
    //
    typedef
    Dispatcher<SemanticGraph::Node>
    NodeDispatcherBase;

    typedef
    Dispatcher<SemanticGraph::Edge>
    EdgeDispatcherBase;


    //
    //
    //
    class NodeDispatcher : public virtual NodeDispatcherBase
    {
    public:
      void
      edge_traverser (EdgeDispatcherBase& d)
      {
        //@@ this should be done in Dispatcher in merge() function?
        //
        for (EdgeDispatcherBase::Iterator i (d.begin ()), end (d.end ());
             i != end; ++i)
        {
          for (EdgeDispatcherBase::Traversers::const_iterator
                 t (i->second.begin ()), end (i->second.end ());
               t != end; ++t)
          {
            dispatcher_.map (i->first, **t);
          }
        }
      }

    protected:
      template <typename I>
      void
      iterate_and_traverse (I begin, I end, EdgeDispatcherBase& d)
      {
        for (; begin != end; ++begin)
        {
          d.dispatch (**begin);
        }
      }

      template <typename I, typename X, typename A>
      void
      iterate_and_traverse (I begin,
                            I end,
                            EdgeDispatcherBase& d,
                            X& x,
                            void (X::*f)(A&),
                            A& a)
      {
        for (I i (begin); i != end;)
        {
          d.dispatch (**i);

          if (++i != end) (x.*f) (a);
        }
      }

      EdgeDispatcherBase&
      edge_traverser ()
      {
        return dispatcher_;
      }

    protected:
      EdgeDispatcherBase dispatcher_;
    };


    class EdgeDispatcher : public virtual EdgeDispatcherBase
    {
    public:
      //@@ this should be done in Dispatcher in merge() function?
      //
      void
      node_traverser (NodeDispatcherBase& d)
      {
        for (NodeDispatcherBase::Iterator i (d.begin ()), end (d.end ());
             i != end; ++i)
        {
          for (NodeDispatcherBase::Traversers::const_iterator
                 t (i->second.begin ()), end (i->second.end ());
               t != end; ++t)
          {
            dispatcher_.map (i->first, **t);
          }
        }
      }

    protected:
      NodeDispatcherBase&
      node_traverser ()
      {
        return dispatcher_;
      }

    protected:
      NodeDispatcherBase dispatcher_;
    };


    //
    //
    //
    template <typename T>
    struct Node : Traverser<SemanticGraph::Node>, virtual NodeDispatcher
    {
      typedef
      T
      Type;

      Node ()
      {
        map (typeid (Type), *this);
      }

      virtual void
      trampoline (SemanticGraph::Node& n)
      {
        //cerr << "trampoline for " << &n << " to type "
        //     << typeid (Type).name () << endl;

        traverse (dynamic_cast<Type&> (n));
      }

      virtual void
      traverse (Type&) = 0;
    };


    template <typename T>
    struct Edge : Traverser <SemanticGraph::Edge>, virtual EdgeDispatcher
    {
      typedef
      T
      Type;

      Edge ()
      {
        map (typeid (Type), *this);
      }

      virtual void
      trampoline (SemanticGraph::Edge& e)
      {
        traverse (dynamic_cast<Type&> (e));
      }


      virtual void
      traverse (Type& e) = 0;
    };

    // Edges
    //
    //
    struct Names : Edge<SemanticGraph::Names>
    {
      virtual void
      traverse (Type& e)
      {
        node_traverser ().dispatch (e.named ());
      }
    };


    struct Belongs : Edge<SemanticGraph::Belongs>
    {
      virtual void
      traverse (Type& e)
      {
        node_traverser ().dispatch (e.type ());
      }
    };


    // Nodes
    //
    //
    struct Nameable : Node<SemanticGraph::Nameable>
    {
    };


    template <typename T>
    struct ScopeTemplate : Node<T>
    {
    public:
      virtual void
      traverse (T& s)
      {
        names (s);
      }

      virtual void
      names (T& s)
      {
        typename T::NamesIterator begin (s.names_begin ()), end (s.names_end ());

        if (begin != end)
        {
          names_pre (s);
          this->iterate_and_traverse (begin, end, this->edge_traverser ());
          names_post (s);
        }
        else
        {
          names_none (s);
        }
      }

      virtual void
      names (T& s, EdgeDispatcherBase& d)
      {
        this->iterate_and_traverse (s.names_begin (), s.names_end (), d);
      }

      virtual void
      names_pre (T&)
      {
      }

      virtual void
      names_post (T&)
      {
      }

      virtual void
      names_none (T&)
      {
      }
    };


    //
    //
    //
    typedef
    ScopeTemplate<SemanticGraph::Scope>
    Scope;


    //
    //
    //
    struct Type : Node<SemanticGraph::Type>
    {
      virtual void
      traverse (SemanticGraph::Type&) = 0;
    };


    //
    //
    //
    struct Instance : Node<SemanticGraph::Instance>
    {
      virtual void
      traverse (Type&);

      virtual void
      pre (Type&);

      virtual void
      belongs (Type&, EdgeDispatcherBase&);

      virtual void
      belongs (Type&);

      virtual void
      post (Type&);
    };

    //
    //
    //
    struct Inherits : Edge<SemanticGraph::Inherits>
    {
      virtual void
      traverse (Type& e)
      {
        node_traverser ().dispatch (e.inheritee ());
      }
    };

    //
    //
    //
    struct Contains : Edge<SemanticGraph::Contains>
    {
      virtual void
      traverse (Type& e)
      {
        node_traverser ().dispatch (e.element ());
      }
    };
  }
}

#include "XSC/Traversal/Elements.tpp"

#endif  // XSC_TRAVERSAL_ELEMENTS_HPP
