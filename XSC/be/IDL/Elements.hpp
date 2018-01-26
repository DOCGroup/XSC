// file      : IDL/Elements.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef IDL_ELEMENTS_HXX
#define IDL_ELEMENTS_HXX

#include <deque>
#include <vector>
#include <string>
#include <ostream>
#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

namespace IDL
{

  using namespace XSC;

  using std::endl;

  typedef std::wstring string;

  //
  //
  class Context
  {
  public:
    typedef std::vector<string> NamespaceMapping;

  public:
    Context (std::wostream& o,
             bool idl3_,
             string const& char_type__,
             NamespaceMapping const& ns_mapping,
             string const& anon_prefix = L"",
             string const& anon_suffix = L"Type")
        : os (o),
          idl3 (idl3_),
          scope (scope_),
          char_type (char_type_),
          L (L_),
          string_type (string_type_),
          parser_type (parser_type_),
          xml_element_type (xml_element_type_),
          xml_attribute_type (xml_attribute_type_),
          char_type_ (char_type__),
          L_ (),
          string_type_ (),
          parser_type_ (),
          xml_element_type_ (),
          xml_attribute_type_ (),
          scope_stack (scope_stack_),
          anon_prefix_ (anon_prefix),
          anon_suffix_ (anon_suffix),
          ns_prefix_ (ns_prefix__),
          ns_suffix_ (ns_suffix__),
          ns_mapping_ (ns_mapping)
    {
      if (char_type == L"wchar_t")
        {
          L_ = L"wchar_t";
        }
      else if (char_type == L"ACE_TCHAR")
        {
          L_ = L"ACE_TEXT";
        }
      else
        {
          L_ = L"";
        }
      string_type_ = L"std::basic_string< " + char_type + L" >";
      parser_type_ = L"::XSCRT::Parser< " + char_type + L" >";
      xml_element_type_ = L"::XSCRT::XML::Element< " + char_type + L" >";
      xml_attribute_type_ = L"::XSCRT::XML::Attribute< " + char_type + L" >";
    }

  protected:
    Context (Context& c)
        : os (c.os),
          idl3 (c.idl3),
          scope (c.scope),
          char_type (c.char_type),
          L (c.L),
          string_type (c.string_type),
          parser_type (c.parser_type),
          xml_element_type (c.xml_element_type),
          xml_attribute_type (c.xml_attribute_type),
          scope_stack (c.scope_stack),
          anon_prefix_ (c.anon_prefix_),
          anon_suffix_ (c.anon_suffix_),
          ns_prefix_ (c.ns_prefix_),
          ns_suffix_ (c.ns_suffix_),
          ns_mapping_ (c.ns_mapping_)
    {
    }


  protected:
    void
    enter_scope (string const& name)
    {
      scope_stack.push_back (name);
      update_scope ();
    }

    void
    leave_scope ()
    {
      scope_stack.pop_back ();
      update_scope ();
    }

  protected:

    // Escape C++ keywords.
    //
    string
    id (string const& name);

    // Translate XML namespace name to C++ identifier.
    //
    string
    ns_name (SemanticGraph::Namespace& ns);

    // Original schema namespace name.
    //
    string
    xs_ns_name (SemanticGraph::Nameable& ns);

    // Mapping for fq-type-name. Takes into account current scope
    // for anonymous names.
    //
    string
    type_name (SemanticGraph::Instance& i, string const& suffix = L"");

    string
    type_name (SemanticGraph::Type& t, string const& suffix = L"");

  protected:
    string
    fq_name (SemanticGraph::Nameable& n, string const& suffix = L"");

  private:
    void
    update_scope ()
    {
      scope.clear ();

      bool first (true);

      for (Scope_::iterator i (scope_stack.begin ());
           i != scope_stack.end ();
           ++i)
      {
        if (first) first = false;
        else scope += L"::";

        scope += *i;
      }
    }

  public:
    string
    ns_prefix () const
    {
      return ns_prefix_;
    }

    void
    ns_prefix (string const& p)
    {
      ns_prefix_ = p;
    }

    string
    ns_suffix () const
    {
      return ns_suffix_;
    }

    void
    ns_suffix (string const& p)
    {
      ns_suffix_ = p;
    }

  public:
    std::wostream& os;

    bool idl3;                  // produce IDL3

    string& scope;

    string& char_type;
    string& L;                  // string literal prefix
    string& string_type;
    string& parser_type;
    string& xml_element_type;
    string& xml_attribute_type;

  private:
    string scope_;

    string char_type_;
    string L_;
    string string_type_;
    string parser_type_;
    string xml_element_type_;
    string xml_attribute_type_;

    typedef
    std::deque<string>
    Scope_;

    Scope_& scope_stack;

    Scope_ scope_stack_;

  protected:
    string anon_prefix_;
    string anon_suffix_;

  protected:
    string ns_prefix__;
    string ns_suffix__;

    string& ns_prefix_;
    string& ns_suffix_;

  protected:
    NamespaceMapping ns_mapping_;
  };


  // Usual namespace mapping.
  //
  struct Namespace : Traversal::Namespace, protected virtual Context
  {
    Namespace (Context& c)
        : Context (c)
    {
    }

    virtual void
    pre (Type& ns);

    virtual void
    post (Type&);
  };


  // Checks if scope `Y' names any of `X'
  //
  template <typename X, typename Y>
  bool
  has (Y& y)
  {
    bool r (false);

    struct Traverser : X
    {
      Traverser (bool& v)
          : v_ (v)
      {
      }

      virtual void
      traverse (typename X::Type&)
      {
        v_ = true;
      }

    private:
      bool& v_;
    } t (r);

    Traversal::Scope s;
    Traversal::Names n;
    s.edge_traverser (n);
    n.node_traverser (t);

    s.traverse (y);

    return r;
  }

  // Fundamental type mapping helper.
  //
  struct Fundamental : Traversal::FundamentalType,
                       Traversal::String,
                       Traversal::NormalizedString,
                       Traversal::Token,
                       Traversal::Name,
                       Traversal::NMTOKEN,
                       Traversal::NCName,
                       Traversal::Id,
                       Traversal::IdRef
  {
    virtual void
    fundamental_type (SemanticGraph::FundamentalType& t) = 0;

    virtual void
    fundamental_template (SemanticGraph::FundamentalType& t) = 0;

    virtual void
    traverse (SemanticGraph::FundamentalType& t)
    {
      fundamental_type (t);
    }

    virtual void
    traverse (SemanticGraph::String& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::NormalizedString& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Token& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Name& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::NMTOKEN& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::NCName& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Id& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::IdRef& t)
    {
      fundamental_template (t);
    }
  };
}

#endif  // IDL_ELEMENTS_HXX
