// file      : CCF/Example/ComponentDeploymentDescriptor/SyntaxTree.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef CDD_SYNTAX_TREE_HPP
#define CDD_SYNTAX_TREE_HPP

#include <set>
#include <vector>
#include <string>


#include "CCF/CompilerElements/ReferenceCounting.hpp"
#include "CCF/CompilerElements/Introspection.hpp"

namespace CDD
{
  namespace SyntaxTree
  {
    using ReferenceCounting::StrictPtr;

    //
    //
    //
    class Node;

    using NodePtr = StrictPtr<Node>;

    class Node : public virtual Introspection::Object,
                 public virtual ReferenceCounting::DefaultImpl <>
    {
    protected:
      virtual
      ~Node () noexcept {}

      Node ()
      {
        type_info (static_type_info ());
      }

    public:
      template <typename Type>
      StrictPtr<Type>
      dynamic_type ()
      {
        NodePtr self (ReferenceCounting::add_ref (this));
        return ReferenceCounting::strict_cast<Type>(self);
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();
    };

    using NodeList = std::vector<NodePtr>;


    //
    //
    //
    class PortInstance : public virtual Node
    {
    public:

      virtual
      ~PortInstance () noexcept {}

      PortInstance (std::string name)
          : name_ (name)
      {
        type_info (static_type_info ());
      }

    public:
      std::string
      name () const
      {
        return name_;
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    private:
      std::string name_;
    };

    using PortInstancePtr = StrictPtr<PortInstance>;

    template <typename T>
    struct NameComparator
    {
      bool
      operator () (T const& x, T const& y) const
      {
        return x->name () < y->name ();
      }
    };

    using PortInstanceSet = std::set<PortInstancePtr, NameComparator<PortInstancePtr>>;


    //
    //
    //
    class ComponentInstance : public virtual Node
    {
    public:
      using Iterator = PortInstanceSet::const_iterator;

    public:

      virtual
      ~ComponentInstance () noexcept {}

      ComponentInstance (std::string name,
                         std::string type)
          : name_ (name),
            type_ (type)
      {
        type_info (static_type_info ());
      }

    public:

      std::string
      name () const
      {
        return name_;
      }

      std::string
      type () const
      {
        return type_;
      }

    public:
      class AlreadyExist {};

      void
      insert (PortInstancePtr const& d)
      {
        if (!ports_.insert (d).second) throw AlreadyExist ();
      }

      Iterator
      begin () const
      {
        return ports_.begin ();
      }


      Iterator
      end () const
      {
        return ports_.end ();
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    private:
      std::string name_;
      std::string type_;

      PortInstanceSet ports_;
    };

    using ComponentInstancePtr = StrictPtr<ComponentInstance>;


    //
    //
    //
    class Connection : public virtual Node
    {
    public:
      virtual
      ~Connection () noexcept {}

      Connection (PortInstancePtr const& left,
                  PortInstancePtr const& right)
          : left_ (left),
            right_ (right)
      {
        type_info (static_type_info ());
      }

    public:

      PortInstancePtr
      left () const
      {
        return left_;
      }

      PortInstancePtr
      right () const
      {
        return right_;
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    private:
      PortInstancePtr left_;
      PortInstancePtr right_;
    };

    using ConnectionPtr = StrictPtr<Connection>;


    //
    //
    //
    class Descriptor : public virtual Node
    {
    public:
      using Iterator = NodeList::const_iterator;

    public:
      virtual
      ~Descriptor () noexcept {}

      Descriptor ()
      {
        type_info (static_type_info ());
      }

    public:
      void
      insert (NodePtr const& n)
      {
        nodes_.push_back (n);
      }

      Iterator
      begin () const
      {
        return nodes_.begin ();
      }


      Iterator
      end () const
      {
        return nodes_.end ();
      }

    public:
      static Introspection::TypeInfo const&
      static_type_info ();

    private:
      NodeList nodes_;
    };

    using DescriptorPtr = StrictPtr<Descriptor>;
  }
}


#endif  // CDD_SYNTAX_TREE_HPP
