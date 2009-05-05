#include "recursive.hpp"

namespace Recursive
{
  // Element
  // 

  Element::
  Element (::std::list< ::XMLSchema::string< wchar_t >::_ptr > const& value__,
           ::std::list< ::XMLSchema::long_::_ptr > const& long___)
  : 
  ::XSCRT::Type (), 
  value_ (value__),
  long__ (long___),
  regulator__ ()
  {
  }

  Element::
  Element (Element const& s)
  :
  ::XSCRT::Type (),
  value_ (s.value_),
  long__ (s.long__),
  el_ (s.el_),
  regulator__ ()
  {
  }

  Element& Element::
  operator= (Element const& s)
  {
    value_ = s.value_;

    long__ = s.long__;

    el_ = s.el_;

    return *this;
  }


  // Element
  // 
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
    return value_.begin ();
  }

  Element::value_const_iterator Element::
  end_value () const
  {
    return value_.end ();
  }

  void Element::
  add_value (::XMLSchema::string< wchar_t >::_ptr const& e)
  {
    value_.push_back (e);
  }

  size_t Element::
  count_value(void) const
  {
    return value_.size ();
  }

  // Element
  // 
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
    return long__.begin ();
  }

  Element::long_const_iterator Element::
  end_long () const
  {
    return long__.end ();
  }

  void Element::
  add_long (::XMLSchema::long_::_ptr const& e)
  {
    long__.push_back (e);
  }

  size_t Element::
  count_long(void) const
  {
    return long__.size ();
  }

  // Element
  // 
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
    return el_.begin ();
  }

  Element::el_const_iterator Element::
  end_el () const
  {
    return el_.end ();
  }

  void Element::
  add_el (::Recursive::Element::_ptr const& e)
  {
    el_.push_back (e);
  }

  size_t Element::
  count_el(void) const
  {
    return el_.size ();
  }
}

namespace Recursive
{
  // Element
  //

  Element::
  Element (::XSCRT::XML::Element< wchar_t > const& e)
  :Base (e), regulator__ ()
  {

    ::XSCRT::Parser< wchar_t > p (e);

    while (p.more_elements ())
    {
      ::XSCRT::XML::Element< wchar_t > e (p.next_element ());
      ::std::basic_string< wchar_t > n (::XSCRT::XML::uq_name (e.name ()));

      if (n == L"value")
      {
        ::XMLSchema::string< wchar_t >::_ptr t (new ::XMLSchema::string< wchar_t > (e));
        add_value (t);
      }

      else if (n == L"long")
      {
        ::XMLSchema::long_::_ptr t (new ::XMLSchema::long_ (e));
        add_long (t);
      }

      else if (n == L"el")
      {
        ::Recursive::Element::_ptr t (new ::Recursive::Element (e));
        add_el (t);
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
      ::XSCRT::XML::Element< wchar_t > e (d->getDocumentElement ());
      if (e.name () == L"el")
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

#include "XMLSchema/TypeInfo.hpp"

namespace Recursive
{
  namespace
  {
    ::XMLSchema::TypeInfoInitializer < wchar_t > XMLSchemaTypeInfoInitializer_ (::XSCRT::extended_type_info_map ());

    struct ElementTypeInfoInitializer
    {
      ElementTypeInfoInitializer ()
      {
        ::XSCRT::TypeId id (typeid (::Recursive::Element));
        ::XSCRT::ExtendedTypeInfo nf (id);

        nf.add_base (::XSCRT::ExtendedTypeInfo::Access::public_, false, typeid (::XSCRT::Type));
        ::XSCRT::extended_type_info_map ().insert (::std::make_pair (id, nf));
      }
    };

    ElementTypeInfoInitializer ElementTypeInfoInitializer_;
  }
}

namespace Recursive
{
  namespace Traversal
  {
    // Element
    //
    //

    void Element::
    traverse (Type& o)
    {
      pre (o);
      value (o);
      long_ (o);
      el (o);
      post (o);
    }

    void Element::
    traverse (Type const& o)
    {
      pre (o);
      value (o);
      long_ (o);
      el (o);
      post (o);
    }

    void Element::
    pre (Type&)
    {
    }

    void Element::
    pre (Type const&)
    {
    }

    void Element::
    value (Type& o)
    {
      // VC6 anathema strikes again
      //
      ::Recursive::Element::value_iterator b (o.begin_value()), e (o.end_value());

      if (b != e)
      {
        value_pre (o);
        for (; b != e;)
        {
          dispatch (*(*b));
          if (++b != e) value_next (o);
        }

        value_post (o);
      }
    }

    void Element::
    value (Type const& o)
    {
      // VC6 anathema strikes again
      //
      ::Recursive::Element::value_const_iterator b (o.begin_value()), e (o.end_value());

      if (b != e)
      {
        value_pre (o);
        for (; b != e;)
        {
          dispatch (*(*b));
          if (++b != e) value_next (o);
        }

        value_post (o);
      }
    }

    void Element::
    value_pre (Type&)
    {
    }

    void Element::
    value_pre (Type const&)
    {
    }

    void Element::
    value_next (Type&)
    {
    }

    void Element::
    value_next (Type const&)
    {
    }

    void Element::
    value_post (Type&)
    {
    }

    void Element::
    value_post (Type const&)
    {
    }

    void Element::
    long_ (Type& o)
    {
      // VC6 anathema strikes again
      //
      ::Recursive::Element::long_iterator b (o.begin_long()), e (o.end_long());

      if (b != e)
      {
        long_pre (o);
        for (; b != e;)
        {
          dispatch (*(*b));
          if (++b != e) long_next (o);
        }

        long_post (o);
      }
    }

    void Element::
    long_ (Type const& o)
    {
      // VC6 anathema strikes again
      //
      ::Recursive::Element::long_const_iterator b (o.begin_long()), e (o.end_long());

      if (b != e)
      {
        long_pre (o);
        for (; b != e;)
        {
          dispatch (*(*b));
          if (++b != e) long_next (o);
        }

        long_post (o);
      }
    }

    void Element::
    long_pre (Type&)
    {
    }

    void Element::
    long_pre (Type const&)
    {
    }

    void Element::
    long_next (Type&)
    {
    }

    void Element::
    long_next (Type const&)
    {
    }

    void Element::
    long_post (Type&)
    {
    }

    void Element::
    long_post (Type const&)
    {
    }

    void Element::
    el (Type& o)
    {
      // VC6 anathema strikes again
      //
      ::Recursive::Element::el_iterator b (o.begin_el()), e (o.end_el());

      if (b != e)
      {
        el_pre (o);
        for (; b != e;)
        {
          dispatch (*(*b));
          if (++b != e) el_next (o);
        }

        el_post (o);
      }

      else el_none (o);
    }

    void Element::
    el (Type const& o)
    {
      // VC6 anathema strikes again
      //
      ::Recursive::Element::el_const_iterator b (o.begin_el()), e (o.end_el());

      if (b != e)
      {
        el_pre (o);
        for (; b != e;)
        {
          dispatch (*(*b));
          if (++b != e) el_next (o);
        }

        el_post (o);
      }

      else el_none (o);
    }

    void Element::
    el_pre (Type&)
    {
    }

    void Element::
    el_pre (Type const&)
    {
    }

    void Element::
    el_next (Type&)
    {
    }

    void Element::
    el_next (Type const&)
    {
    }

    void Element::
    el_post (Type&)
    {
    }

    void Element::
    el_post (Type const&)
    {
    }

    void Element::
    el_none (Type&)
    {
    }

    void Element::
    el_none (Type const&)
    {
    }

    void Element::
    post (Type&)
    {
    }

    void Element::
    post (Type const&)
    {
    }
  }
}

namespace Recursive
{
  namespace Writer
  {
    // Element
    //
    //

    Element::
    Element (::XSCRT::XML::Element< wchar_t >& e)
    : ::XSCRT::Writer< wchar_t > (e)
    {
    }

    Element::
    Element ()
    {
    }

    void Element::
    traverse (Type const& o)
    {
      Traversal::Element::traverse (o);
    }

    void Element::
    value_pre (Type const&)
    {
      push_ (::XSCRT::XML::Element< wchar_t > (L"value", top_ ()));
    }

    void Element::
    value_next (Type const& o)
    {
      value_post (o);
      value_pre (o);
    }

    void Element::
    value_post (Type const&)
    {
      pop_ ();
    }

    void Element::
    long_pre (Type const&)
    {
      push_ (::XSCRT::XML::Element< wchar_t > (L"long", top_ ()));
    }

    void Element::
    long_next (Type const& o)
    {
      long_post (o);
      long_pre (o);
    }

    void Element::
    long_post (Type const&)
    {
      pop_ ();
    }

    void Element::
    el_pre (Type const&)
    {
      push_ (::XSCRT::XML::Element< wchar_t > (L"el", top_ ()));
    }

    void Element::
    el_next (Type const& o)
    {
      el_post (o);
      el_pre (o);
    }

    void Element::
    el_post (Type const&)
    {
      pop_ ();
    }
  }
}

namespace Recursive
{
  namespace writer
  {
    void
    el (::Recursive::Element const& s, xercesc::DOMDocument* d)
    {
      ::XSCRT::XML::Element< wchar_t > e (d->getDocumentElement ());
      if (e.name () != L"el")
      {
        throw 1;
      }

      struct W : virtual ::Recursive::Writer::Element,
      virtual ::XMLSchema::Writer::FundamentalType< ::XMLSchema::string< wchar_t >, wchar_t >,
      virtual ::XMLSchema::Writer::FundamentalType< ::XMLSchema::long_, wchar_t >,
      virtual ::XSCRT::Writer< wchar_t >
      {
        W (::XSCRT::XML::Element< wchar_t >& e)
        : ::XSCRT::Writer< wchar_t > (e)
        {
        }
      };

      W w (e);
      w.dispatch (s);
    }
  }
}

