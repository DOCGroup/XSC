#include "recursive.hpp"

#include "ace/ace_wchar.h"
namespace Recursive
{
  // Element

  Element::Element (value_container_type const& value__,
                    long_container_type const& long___)
  : ::XSCRT::Type ()
  , value_ (value__)
  , long__ (long___)
  {
  }

  Element::Element (Element const& s)
  : ::XSCRT::Type (s)
  , value_ (s.value_)
  , long__ (s.long__)
  , el_ (s.el_)
  {
  }

  Element&
  Element::operator= (Element const& s)
  {
    if (std::addressof(s) != this)
    {
      value_ = s.value_;

      long__ = s.long__;

      el_ = s.el_;
    }

    return *this;
  }


  // Element
  Element::value_iterator Element::
  begin_value ()
  {
    return value_.begin ();
  }

  Element::value_iterator Element::
  end_value ()
  {
    return value_.end ();
  }

  Element::value_const_iterator Element::
  begin_value () const
  {
    return value_.cbegin ();
  }

  Element::value_const_iterator Element::
  end_value () const
  {
    return value_.cend ();
  }

  size_t Element::
  count_value() const
  {
    return value_.size ();
  }

  // Element
  Element::long_iterator Element::
  begin_long ()
  {
    return long__.begin ();
  }

  Element::long_iterator Element::
  end_long ()
  {
    return long__.end ();
  }

  Element::long_const_iterator Element::
  begin_long () const
  {
    return long__.cbegin ();
  }

  Element::long_const_iterator Element::
  end_long () const
  {
    return long__.cend ();
  }

  size_t Element::
  count_long() const
  {
    return long__.size ();
  }

  // Element
  Element::el_iterator Element::
  begin_el ()
  {
    return el_.begin ();
  }

  Element::el_iterator Element::
  end_el ()
  {
    return el_.end ();
  }

  Element::el_const_iterator Element::
  begin_el () const
  {
    return el_.cbegin ();
  }

  Element::el_const_iterator Element::
  end_el () const
  {
    return el_.cend ();
  }

  size_t Element::
  count_el() const
  {
    return el_.size ();
  }
}

namespace Recursive
{
  // Element

  Element::
  Element (::XSCRT::XML::Element<ACE_TCHAR> const& element)
  :Base (element)
  {

    ::XSCRT::Parser<ACE_TCHAR> p (element);

    while (p.more_elements ())
    {
      ::XSCRT::XML::Element<ACE_TCHAR> e (p.next_element ());
      std::basic_string<ACE_TCHAR> n (::XSCRT::XML::uq_name (e.name ()));

      if (n == ACE_TEXT("value"))
      {
        ::XMLSchema::string<ACE_TCHAR> t (e);
        value_.push_back (t);
      }

      else if (n == ACE_TEXT("long"))
      {
        ::XMLSchema::long_ t (e);
        long__.push_back (t);
      }

      else if (n == ACE_TEXT("el"))
      {
        ::Recursive::Element t (e);
        el_.push_back (t);
      }

      else
      {
      }
    }
  }
}

namespace Recursive
{
  namespace reader
  {
    ::Recursive::Element
    el (xercesc::DOMDocument const* d)
    {
      xercesc::DOMElement* dom_element = d->getDocumentElement ();
      if (!dom_element)
      {
        throw 1;
      }

      ::XSCRT::XML::Element<ACE_TCHAR> e (dom_element);
      if (e.name () == ACE_TEXT("el"))
      {
        ::Recursive::Element r (e);

        return r;
      }

      else
      {
        throw 1;
      }
    }
  }
}

