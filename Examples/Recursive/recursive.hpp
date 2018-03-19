#ifndef RECURSIVE_HPP
#define RECURSIVE_HPP

// Forward declarations.
namespace Recursive
{
  class Element;
}

#include <memory>
#include <string>
#include <list>
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XMLSchema/id_map.hpp"
#include "ace/Refcounted_Auto_Ptr.h"
#include "ace/Null_Mutex.h"
#include "ace/TSS_T.h"
#include "ace/ace_wchar.h"
#include "ace/Singleton.h"

namespace Recursive
{
  class Element : public ::XSCRT::Type
  {
    typedef ::XSCRT::Type Base;

    public:
    typedef ACE_Refcounted_Auto_Ptr < ::Recursive::Element, ACE_Null_Mutex> _ptr;

    // value
    public:
    typedef ACE_Refcounted_Auto_Ptr < ::XMLSchema::string<ACE_TCHAR>, ACE_Null_Mutex> value_value_type;
    typedef std::list<value_value_type> value_container_type;
    typedef value_container_type::iterator value_iterator;
    typedef value_container_type::const_iterator value_const_iterator;
    value_iterator begin_value ();
    value_iterator end_value ();
    value_const_iterator begin_value () const;
    value_const_iterator end_value () const;
    void add_value (value_value_type const&);
    size_t count_value () const;

    protected:
    value_container_type value_;

    // long
    public:
    typedef ACE_Refcounted_Auto_Ptr < ::XMLSchema::long_, ACE_Null_Mutex> long_value_type;
    typedef std::list<long_value_type> long_container_type;
    typedef long_container_type::iterator long_iterator;
    typedef long_container_type::const_iterator long_const_iterator;
    long_iterator begin_long ();
    long_iterator end_long ();
    long_const_iterator begin_long () const;
    long_const_iterator end_long () const;
    void add_long (long_value_type const&);
    size_t count_long () const;

    protected:
    long_container_type long__;

    // el
    public:
    typedef ACE_Refcounted_Auto_Ptr < ::Recursive::Element, ACE_Null_Mutex> el_value_type;
    typedef std::list<el_value_type> el_container_type;
    typedef el_container_type::iterator el_iterator;
    typedef el_container_type::const_iterator el_const_iterator;
    el_iterator begin_el ();
    el_iterator end_el ();
    el_const_iterator begin_el () const;
    el_const_iterator end_el () const;
    void add_el (el_value_type const&);
    size_t count_el () const;

    protected:
    el_container_type el_;

    public:
    Element (value_container_type const& value__,
             long_container_type const& long___);

    explicit Element (::XSCRT::XML::Element<ACE_TCHAR> const&);
    Element (Element const& s);
    Element& operator= (Element const& s);

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

#include "ace/XML_Utils/XMLSchema/Traversal.hpp"

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

#include "ace/XML_Utils/XMLSchema/Writer.hpp"

namespace Recursive
{
  namespace Writer
  {
    struct Element : Traversal::Element,
    virtual ::XSCRT::Writer<ACE_TCHAR>
    {
      typedef ::Recursive::Element Type;
      explicit Element (::XSCRT::XML::Element<ACE_TCHAR>&);

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
