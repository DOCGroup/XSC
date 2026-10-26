// file      : CXX/Elements.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef CXX_ELEMENTS_HXX
#define CXX_ELEMENTS_HXX

#include <deque>
#include <functional>
#include <vector>
#include <string>
#include <ostream>

#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"
#include "CCF/CodeGenerationKit/IndentationImplanter.hpp"

using namespace XSC;

using string = std::wstring;

using NamespaceMapping = std::vector<string>;

//
class Context
{
public:
  // Which C++ version we generate for
  enum class CPPMODE {
    CPP03,
    CPP11,
    CPP17
  };

  enum class ORB {
    TAO,
    TAOX11
  };


public:
  Context (std::wostream& o,
           string const& char_type__,
           string const& esymbol__,
           NamespaceMapping const& ns_mapping,
           string const& anon_prefix = L"",
           string const& anon_suffix = L"Type")
      : os (o),
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
        hxx_expr_ (),
        hxx_expr (hxx_expr_),
        hxx_suffix_ (),
        hxx_suffix (hxx_suffix_),
        esymbol_ (esymbol__),
        esymbol (esymbol_),
        scope_stack (scope_stack_),
        anon_prefix_ (anon_prefix),
        anon_suffix_ (anon_suffix),
        ns_prefix_ (ns_prefix__),
        ns_suffix_ (ns_suffix__),
        ns_mapping_ (ns_mapping),
        cdr_reader_generation_ (false),
        cdr_writer_generation_ (false),
        cppmode_ (CPPMODE::CPP17),
        orb_ (ORB::TAO),
        generate_ra_sequences_ (false),
        read_only_ (false)
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
    string_type_ = L"std::basic_string<" + char_type + L">";
    if (char_type == L"char")
      {
        string_type_ = L"std::string";
      }
    parser_type_ = L"::XSCRT::Parser<" + char_type + L">";
    xml_element_type_ = L"::XSCRT::XML::Element<" + char_type + L">";
    xml_attribute_type_ = L"::XSCRT::XML::Attribute<" + char_type + L">";
  }

protected:
  Context (Context& c)
      : os (c.os),
        scope (c.scope),
        char_type (c.char_type),
        L (c.L),
        string_type (c.string_type),
        parser_type (c.parser_type),
        xml_element_type (c.xml_element_type),
        xml_attribute_type (c.xml_attribute_type),
        hxx_expr (c.hxx_expr),
        hxx_suffix (c.hxx_suffix),
        esymbol (c.esymbol),
        scope_stack (c.scope_stack),
        anon_prefix_ (c.anon_prefix_),
        anon_suffix_ (c.anon_suffix_),
        ns_prefix_ (c.ns_prefix_),
        ns_suffix_ (c.ns_suffix_),
        ns_mapping_ (c.ns_mapping_),
        cdr_reader_generation_ (c.cdr_reader_generation_),
        cdr_writer_generation_ (c.cdr_writer_generation_),
        cppmode_ (c.cppmode_),
        orb_ (c.orb_),
        generate_ra_sequences_ (c.generate_ra_sequences_),
        read_only_ (c.read_only_)
  {
  }


protected:
  void
  enter_scope (string const& name)
  {
    scope_stack.push_back (name);
    update_scope ();
  }

  size_t
  scope_depth ()
  {
    return scope_stack.size ();
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
  type_name (SemanticGraph::Instance& i, string const& ns_suffix = L"");

  string
  type_name (SemanticGraph::Type& t, string const& ns_suffix = L"");

  string
  fundamental_type_name (SemanticGraph::FundamentalType& t);

  string
  base_type_name (SemanticGraph::Complex& t);

  string
  sequence_accessor_name (SemanticGraph::Element& e);

protected:
  string
  fq_name (SemanticGraph::Nameable& n, string const& ns_suffix = L"", bool mangle = true);

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
  void
  cdr_reader_generation (bool flag)
  {
    this->cdr_reader_generation_ = flag;
  }
  void
  cdr_writer_generation (bool flag)
  {
    this->cdr_writer_generation_ = flag;
  }
  void
  cppmode (CPPMODE flag)
  {
    this->cppmode_ = flag;
  }
  bool
  cdr_reader_generation_enabled ()
  {
    return this->cdr_reader_generation_;
  }
  bool
  cdr_writer_generation_enabled ()
  {
    return this->cdr_writer_generation_;
  }
  CPPMODE
  cppmode ()
  {
    return this->cppmode_;
  }

  void
  orb (ORB value)
  {
    this->orb_ = value;
  }

  ORB
  orb ()
  {
    return this->orb_;
  }

  void
  generate_ra_sequences (bool flag)
  {
    this->generate_ra_sequences_ = flag;
  }

  bool
  generate_ra_sequences ()
  {
    return this->generate_ra_sequences_;
  }

  void
  read_only (bool flag)
  {
    this->read_only_ = flag;
  }

  bool
  read_only ()
  {
    return this->read_only_;
  }

  bool
  optional_via_if_set ()
  {
    return this->cppmode_ != CPPMODE::CPP03 || this->read_only_;
  }

  string
  string_literal (string const& value)
  {
    if (L.empty ())
      return L"\"" + value + L"\"";

    return L + L"(\"" + value + L"\")";
  }

public:
  std::wostream& os;
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

  std::string hxx_expr_;

public:
  std::string &hxx_expr;

private:
  std::string hxx_suffix_;

public:
  std::string &hxx_suffix;

private:
  string esymbol_;

public:
  string &esymbol;

private:
  using Scope_ = std::deque<string>;

  Scope_ scope_stack_;

  Scope_ &scope_stack;

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

//-- CDR Insertion and Extraction Operators --
  bool cdr_reader_generation_;
  bool cdr_writer_generation_;

  // Generate code depending on C++ features
  CPPMODE cppmode_;

  ORB orb_;

  bool generate_ra_sequences_;
  bool read_only_;
};

struct Anonymous : Traversal::Element,
                   Traversal::Complex,
                   protected virtual Context

{
  Anonymous (Context& c, Traversal::NodeDispatcher& d1)
      : Context (c), complex_ (c, &d1, nullptr)
  {
    edge_traverser (names_);
    names_.node_traverser (*this);

    belongs_.node_traverser (complex_);
    complex_.edge_traverser (names_);
  }

  Anonymous (Context& c,
             Traversal::NodeDispatcher& d1,
             Traversal::NodeDispatcher& d2)
      : Context (c), complex_ (c, &d1, &d2)
  {
    edge_traverser (names_);
    names_.node_traverser (*this);

    belongs_.node_traverser (complex_);
    complex_.edge_traverser (names_);
  }

  void
  traverse (SemanticGraph::Element& e) override
  {
    SemanticGraph::Type& t (e.type ());

    if (!t.named () && !t.context ().count ("seen"))
    {
      t.context ().set ("seen", true);

      enter_scope (e.name ());

      Element::belongs (e, belongs_);

      leave_scope ();

      t.context ().remove ("seen");
    }
  }

private:
  struct Complex : Traversal::Complex,
                   protected virtual Context
  {
    Complex (Context& c,
             Traversal::NodeDispatcher* d1,
             Traversal::NodeDispatcher* d2)
        : Context (c), d1_ (d1), d2_ (d2)
    {
    }

    void
    traverse (SemanticGraph::Complex& c) override
    {
      c.context ().set ("name", scope);

      if (d1_) d1_->dispatch (c);
      if (d2_) d2_->dispatch (c);

      c.context ().remove ("name");

      Traversal::Complex::traverse (c);
    }

  private:
    Traversal::NodeDispatcher* d1_;
    Traversal::NodeDispatcher* d2_;

  } complex_;


  Traversal::Names names_;

  Traversal::Belongs belongs_;
};


// Usual namespace mapping.
//
struct Namespace : Traversal::Namespace, protected virtual Context
{
  Namespace (Context& c)
      : Context (c)
  {
  }

  void
  pre (Type& ns) override;

  void
  post (Type&) override;
};

template <typename T>
struct Traverser : T
{
  Traverser (bool& v)
      : v_ (v)
  {
  }

  void
  traverse (typename T::Type &) override
  {
    v_ = true;
  }

private:
  bool& v_;
};

// Checks if scope `Y' names any of `X'
//
template <typename X, typename Y>
bool
has (Y& y)
{
  bool r (false);

  Traverser <X> t (r);
  Traversal::Scope s;
  Traversal::Names n;
  s.edge_traverser (n);
  n.node_traverser (t);

  s.traverse (y);

  return r;
}

struct CopyRequiredMembers : Traversal::Element, Traversal::Attribute
{
  void
  traverse (SemanticGraph::Element& e) override
  {
    if (e.max () == 1)
      required = true;
  }

  void
  traverse (SemanticGraph::Attribute&) override
  {
    required = true;
  }

  bool required {};
};

inline bool
has_copy_required_members (SemanticGraph::Complex& c)
{
  CopyRequiredMembers t;
  Traversal::Scope s;
  Traversal::Names n;
  s.edge_traverser (n);
  n.node_traverser (t);
  s.traverse (c);
  return t.required;
}

struct ConstructorRequiredMembers : Traversal::Enumeration,
                                   Traversal::Element,
                                   Traversal::Attribute
{
  void
  traverse (SemanticGraph::Enumeration&) override
  {
    required = true;
  }

  void
  traverse (SemanticGraph::Element& e) override
  {
    if (e.min () >= 1)
      required = true;
  }

  void
  traverse (SemanticGraph::Attribute& a) override
  {
    if (!a.optional ())
      required = true;
  }

  bool required {};
};

inline bool
has_constructor_required_members (SemanticGraph::Complex& c)
{
  ConstructorRequiredMembers t;

  std::function<void (SemanticGraph::Complex&)> visit = [&] (SemanticGraph::Complex& current)
  {
    for (auto i (current.inherits_begin ()); i != current.inherits_end (); ++i)
      if (auto base = dynamic_cast<SemanticGraph::Complex*> (std::addressof ((**i).inheritee ())))
        visit (*base);

    Traversal::Complex complex;
    Traversal::Names names;
    complex.edge_traverser (names);
    names.node_traverser (t);
    complex.traverse (current);
  };

  visit (c);
  return t.required;
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
                     Traversal::IdRef,
                     Traversal::QName,
                     Traversal::anyURI
  {
    virtual void
    fundamental_type (SemanticGraph::FundamentalType& t) = 0;

    virtual void
    fundamental_template (SemanticGraph::FundamentalType& t) = 0;

    void
    traverse (SemanticGraph::FundamentalType& t) override
    {
      fundamental_type (t);
    }

    void
    traverse (SemanticGraph::String& t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::NormalizedString& t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::Token& t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::Name& t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::NMTOKEN& t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::NCName& t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::QName& t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::AnyUri & t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::Id& t) override
    {
      fundamental_template (t);
    }

    void
    traverse (SemanticGraph::IdRef& t) override
    {
      fundamental_template (t);
    }
  };

#endif  // CXX_ELEMENTS_HXX
