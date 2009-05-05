#ifndef RECURSIVE_HPP
#define RECURSIVE_HPP

// Forward declarations.
//
namespace Recursive
{
  class Element;
}

#include <memory>
#include <list>
#include "XMLSchema/Types.hpp"

#include "ace/Refcounted_Auto_Ptr.h"
#include "ace/Null_Mutex.h"

namespace Recursive
{
  class Element : public ::XSCRT::Type
  {
    typedef ::XSCRT::Type Base;

    typedef ACE_Refcounted_Auto_Ptr < Element, ACE_Null_Mutex > _ptr;

    // value
    // 
    public:
    typedef ::std::list< ::XMLSchema::string< wchar_t >::_ptr >::iterator value_iterator;
    typedef ::std::list< ::XMLSchema::string< wchar_t >::_ptr >::const_iterator value_const_iterator;
    value_iterator begin_value ();
    value_iterator end_value ();
    value_const_iterator begin_value () const;
    value_const_iterator end_value () const;
    void add_value (::XMLSchema::string< wchar_t >::_ptr const& );
    size_t count_value (void) const;

    protected:
    ::std::list< ::XMLSchema::string< wchar_t >::_ptr > value_;

    // long
    // 
    public:
    typedef ::std::list< ::XMLSchema::long_::_ptr >::iterator long_iterator;
    typedef ::std::list< ::XMLSchema::long_::_ptr >::const_iterator long_const_iterator;
    long_iterator begin_long ();
    long_iterator end_long ();
    long_const_iterator begin_long () const;
    long_const_iterator end_long () const;
    void add_long (::XMLSchema::long_::_ptr const& );
    size_t count_long (void) const;

    protected:
    ::std::list< ::XMLSchema::long_::_ptr > long__;

    // el
    // 
    public:
    typedef ::std::list< ::Recursive::Element::_ptr >::iterator el_iterator;
    typedef ::std::list< ::Recursive::Element::_ptr >::const_iterator el_const_iterator;
    el_iterator begin_el ();
    el_iterator end_el ();
    el_const_iterator begin_el () const;
    el_const_iterator end_el () const;
    void add_el (::Recursive::Element::_ptr const& );
    size_t count_el (void) const;

    protected:
    ::std::list< ::Recursive::Element::_ptr > el_;

    public:
    Element (::std::list< ::XMLSchema::string< wchar_t >::_ptr > const& value__,
             ::std::list< ::XMLSchema::long_::_ptr > const& long___);

    Element (::XSCRT::XML::Element< wchar_t > const&);
    Element (Element const& s);

    Element&
    operator= (Element const& s);

    private:
    char regulator__;
  };
}

namespace Recursive
{
  namespace reader
  {
    ::Recursive::Element
    el (xercesc::DOMDocument const*);
  }
}

#include "XMLSchema/Traversal.hpp"

namespace Recursive
{
  namespace Traversal
  {
    struct Element : ::XMLSchema::Traversal::Traverser< ::Recursive::Element >
    {
      virtual void
      traverse (Type&);

      virtual void
      traverse (Type const&);

      virtual void
      pre (Type&);

      virtual void
      pre (Type const&);

      virtual void
      value (Type&);

      virtual void
      value (Type const&);

      virtual void
      value_pre (Type&);

      virtual void
      value_pre (Type const&);

      virtual void
      value_next (Type&);

      virtual void
      value_next (Type const&);

      virtual void
      value_post (Type&);

      virtual void
      value_post (Type const&);

      virtual void
      long_ (Type&);

      virtual void
      long_ (Type const&);

      virtual void
      long_pre (Type&);

      virtual void
      long_pre (Type const&);

      virtual void
      long_next (Type&);

      virtual void
      long_next (Type const&);

      virtual void
      long_post (Type&);

      virtual void
      long_post (Type const&);

      virtual void
      el (Type&);

      virtual void
      el (Type const&);

      virtual void
      el_pre (Type&);

      virtual void
      el_pre (Type const&);

      virtual void
      el_next (Type&);

      virtual void
      el_next (Type const&);

      virtual void
      el_post (Type&);

      virtual void
      el_post (Type const&);

      virtual void
      el_none (Type&);

      virtual void
      el_none (Type const&);

      virtual void
      post (Type&);

      virtual void
      post (Type const&);
    };
  }
}

#include "XMLSchema/Writer.hpp"

namespace Recursive
{
  namespace Writer
  {
    struct Element : Traversal::Element, 
    virtual ::XSCRT::Writer< wchar_t >
    {
      typedef ::Recursive::Element Type;
      Element (::XSCRT::XML::Element< wchar_t >&);

      virtual void 
      traverse (Type &o)
      {
        this->traverse (const_cast <Type const &> (o));
      }

      virtual void
      traverse (Type const&);

      virtual void 
      value_pre (Type &o)
      {
        this->value_pre (const_cast <Type const &> (o));
      }

      virtual void
      value_pre (Type const&);

      virtual void 
      value_next (Type &o)
      {
        this->value_next (const_cast <Type const &> (o));
      }

      virtual void
      value_next (Type const&);

      virtual void 
      value_post (Type &o)
      {
        this->value_post (const_cast <Type const &> (o));
      }

      virtual void
      value_post (Type const&);

      virtual void 
      long_pre (Type &o)
      {
        this->long_pre (const_cast <Type const &> (o));
      }

      virtual void
      long_pre (Type const&);

      virtual void 
      long_next (Type &o)
      {
        this->long_next (const_cast <Type const &> (o));
      }

      virtual void
      long_next (Type const&);

      virtual void 
      long_post (Type &o)
      {
        this->long_post (const_cast <Type const &> (o));
      }

      virtual void
      long_post (Type const&);

      virtual void 
      el_pre (Type &o)
      {
        this->el_pre (const_cast <Type const &> (o));
      }

      virtual void
      el_pre (Type const&);

      virtual void 
      el_next (Type &o)
      {
        this->el_next (const_cast <Type const &> (o));
      }

      virtual void
      el_next (Type const&);

      virtual void 
      el_post (Type &o)
      {
        this->el_post (const_cast <Type const &> (o));
      }

      virtual void
      el_post (Type const&);

      protected:
      Element ();
    };
  }
}

namespace Recursive
{
  namespace writer
  {
    void
    el (::Recursive::Element const&, xercesc::DOMDocument*);
  }
}

#endif // RECURSIVE_HPP
