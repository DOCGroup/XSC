// file      : IDL/Elements.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "Elements.hpp"
#include "CCF/CodeGenerationKit/Regex.hpp"
#include "XSC/Traversal.hpp"

#include <sstream>

namespace IDL
{

  // Namespace
  //
  //

  string Context::
  ns_name (SemanticGraph::Namespace& ns)
  {
    using SemanticGraph::Schema;
    using SemanticGraph::Includes;
    using SemanticGraph::Imports;
    using SemanticGraph::Implies;

    string name, r;

    Schema& schema (dynamic_cast<Schema&> (ns.scope ()));

    if (schema.contained_p ())
    {
      SemanticGraph::Contains& c (schema.contained ());

      fs::path f;

      if (Includes* i = dynamic_cast<Includes*> (&c)) f = i->file (); else
        if (Imports* i = dynamic_cast<Imports*> (&c)) f = i->file (); else
          if (Implies* i = dynamic_cast<Implies*> (&c)) f = i->file ();

      if (!f.empty ())
      {
        std::wostringstream ostr;
        ostr << f.string ().c_str ();
        name = ostr.str ();
      }
    }

    name += L' ' + ns.name ();

    for (NamespaceMapping::reverse_iterator e (ns_mapping_.rbegin ());
         e != ns_mapping_.rend (); ++e)
    {
      r = regex::perl_s (name, *e);
      r = regex::perl_s (r, L"#/#::#"); // replace `/' with `::'

      // Check the result.
      //
      if (regex::perl_s (
            r, L"/^([a-zA-Z_]\\w*)(::[a-zA-Z_]\\w*)*$//").empty ()) break;
    }

    if (!ns_prefix_.empty ()) r = ns_prefix_ + L"::" + r;
    if (!ns_suffix_.empty ()) r += L"::" + ns_suffix_;

    return r;
  }

  string Context::
  xs_ns_name (SemanticGraph::Nameable& n)
  {
    using namespace SemanticGraph;

    Scope& s (n.scope ());

    SemanticGraph::Namespace* ns (dynamic_cast<SemanticGraph::Namespace*> (&n));

    return ns ? ns->name () : xs_ns_name (s);
  }

  string Context::
  fq_name (SemanticGraph::Nameable& n, string const& suffix)
  {
    using namespace SemanticGraph;

    string r;

    Scope& s (n.scope ());

    if (!dynamic_cast<Schema*> (&s)) r = fq_name (s, suffix);

    if (SemanticGraph::Namespace* ns =
        dynamic_cast<SemanticGraph::Namespace*> (&n))
    {
      string name (ns_name (*ns));

      if (!name.empty ())
      {
        r += L"::";
        r += name;
      }
    }
    else
    {
      r += L"::";
      r += id (n.name () + suffix);
    }

    return r;
  }

  namespace
  {
    //@@ recode using Fundamental
    //
    struct TypeName : protected virtual Context,
                      Traversal::Type
                      /*
                        public Traversal::Byte,
                        public Traversal::UnsignedByte,
                        public Traversal::Short,
                        public Traversal::UnsignedShort,
                        public Traversal::Int,
                        public Traversal::UnsignedInt,
                        public Traversal::Long,
                        public Traversal::UnsignedLong,
                        public Traversal::Boolean,
                        public Traversal::Float,
                        public Traversal::Double,

                      Traversal::String,
                      Traversal::NormalizedString,
                      Traversal::Token,
                      Traversal::Name,
                      Traversal::NMTOKEN,
                      Traversal::NCName,
                      Traversal::Id,
                      Traversal::IdRef
                      */

    {
      TypeName (Context& c, string& r_, string const& suffix)
          : Context (c), r (r_), suffix_ (suffix)
      {
      }

      virtual void
      traverse (SemanticGraph::Type& t)
      {
        r  = fq_name (t, suffix_);
      }

      /*
        virtual void
        traverse (SemanticGraph::Byte&)
        {
        r = L"char";
        }

        virtual void
        traverse (SemanticGraph::UnsignedByte&)
        {
        r = L"unsigned char";
        }

        virtual void
        traverse (SemanticGraph::Short&)
        {
        r = L"short";
        }

        virtual void
        traverse (SemanticGraph::UnsignedShort&)
        {
        r = L"unsigned short";
        }

        virtual void
        traverse (SemanticGraph::Int&)
        {
        r = L"int";
        }

        virtual void
        traverse (SemanticGraph::UnsignedInt&)
        {
        r = L"unsigned int";
        }

        virtual void
        traverse (SemanticGraph::Long&)
        {
        r = L"long";
        }

        virtual void
        traverse (SemanticGraph::UnsignedLong&)
        {
        r = L"unsigned long";
        }

        virtual void
        traverse (SemanticGraph::Boolean&)
        {
        r = L"bool";
        }

        virtual void
        traverse (SemanticGraph::Float&)
        {
        r = L"float";
        }

        virtual void
        traverse (SemanticGraph::Double&)
        {
        r = L"double";
        }


      virtual void
      traverse (SemanticGraph::String&)
      {
        r = schema_ns () + L"::string< " + char_type + L" >";
      }

      virtual void
      traverse (SemanticGraph::NormalizedString&)
      {
        r = schema_ns () + L"::normalizedString< " + char_type + L" >";
      }

      virtual void
      traverse (SemanticGraph::Token&)
      {
        r = schema_ns () + L"::token< " + char_type + L" >";
      }

      virtual void
      traverse (SemanticGraph::Name&)
      {
        r = schema_ns () + L"::Name< " + char_type + L" >";
      }

      virtual void
      traverse (SemanticGraph::NMTOKEN&)
      {
        r = schema_ns () + L"::NMTOKEN< " + char_type + L" >";
      }

      virtual void
      traverse (SemanticGraph::NCName&)
      {
        r = schema_ns () + L"::NCName< " + char_type + L" >";
      }

      virtual void
      traverse (SemanticGraph::Id&)
      {
        r = schema_ns () + L"::ID< " + char_type + L" >";
      }

      virtual void
      traverse (SemanticGraph::IdRef&)
      {
        r = schema_ns () + L"::IDREF< " + char_type + L" >";
      }
      */

    private:
      string
      schema_ns ()
      {
        return L"::XMLSchema";
      }

    private:
      string& r;
      string const& suffix_;
    };
  }

  //@@ In what situations would I produce different name by
  //   these two functions?
  //
  string Context::
  type_name (SemanticGraph::Type& t, string const& suffix)
  {
    // There cannot be a named type inside an anonymous one.
    //
    if (t.named ())
    {
      string r;

      TypeName tn (*this, r, suffix);
      tn.dispatch (t);

      return r;
    }

    else
      {
        wcerr << "There can't be a named type inside an anonymous one!\n";
        throw;
      }


  }

  string Context::
  type_name (SemanticGraph::Instance& i, string const& suffix)
  {
    SemanticGraph::Type& t (i.type ());

    return type_name (t, suffix);
  }

  namespace
  {
    const wchar_t* idl2_keywords[] = {
      L"abstract",
      L"any",
      L"attribute",
      L"boolean",
      L"case",
      L"char",
      L"const",
      L"context",
      L"custom",
      L"default",
      L"double",
      L"enum",
      L"exception",
      L"factory",
      L"FALSE",
      L"fixed",
      L"float",
      L"getraises",
      L"in",
      L"inout",
      L"interface",
      L"local",
      L"long",
      L"module",
      L"native",
      L"Object",
      L"octet",
      L"oneway",
      L"out",
      L"private",
      L"public",
      L"raises",
      L"readonly",
      L"sequence",
      L"setraises",
      L"short",
      L"string",
      L"struct",
      L"supports",
      L"switch",
      L"TRUE",
      L"truncatable",
      L"typedef",
      L"typeid",
      L"typeprefix",
      L"union",
      L"unsigned",
      L"ValueBase",
      L"valuetype",
      L"void",
      L"wchar",
      L"wstring"
    };

    const wchar_t* idl3_keywords[] = {
      L"component",
      L"consumes",
      L"emits",
      L"eventtype",
      L"finder",
      L"home",
      L"import",
      L"multiple",
      L"primarykey",
      L"provides",
      L"publishes",
      L"uses"
    };

    const wchar_t* cxx_keywords[] = {
      L"int"
    };


    struct comparator
    {
      bool
      operator() (wchar_t const* a, wchar_t const* b) const
      {
        wchar_t A (towupper (*a)), B (towupper (*b));

        while (A != L'\0')
        {
          if (A < B) return true;
          if (A > B) return false;

          ++a;
          ++b;

          A = towupper (*a);
          B = towupper (*b);
        }

        return A < B;
      }
    };
  }

  string Context::
  id (string const& name)
  {
    comparator cmp;

    {
      std::size_t const size (sizeof (idl2_keywords) / sizeof (wchar_t*));

      if (std::binary_search (idl2_keywords,
                              idl2_keywords + size,
                              name.c_str (),
                              cmp)) return name + L'_';
    }

    {
      std::size_t const size (sizeof (cxx_keywords) / sizeof (wchar_t*));

      if (std::binary_search (cxx_keywords,
                              cxx_keywords + size,
                              name.c_str (),
                              cmp)) return name + L'_';
    }

    if (idl3)
    {
      std::size_t const size (sizeof (idl3_keywords) / sizeof (wchar_t*));

      if (std::binary_search (idl3_keywords,
                              idl3_keywords + size,
                              name.c_str (),
                              cmp)) return name + L'_';
    }

    return name;
  }


  // Namespace
  //
  //
  void Namespace::
  pre (Type& n)
  {
    string ns (ns_name (n));

    string::size_type b (0), e;

    do
    {
      e = ns.find (L"::", b);

      string t (ns, b, e == ns.npos ? e : e - b);

      if (!t.empty ()) os << "module " << id (t) << "{";

      b = e;

      if (b == ns.npos) break;

      b += 2;
    }
    while (true);
  }

  void Namespace::
  post (Type& n)
  {
    string ns (ns_name (n));

    string::size_type b (0), e;

    do
    {
      e = ns.find (L"::", b);

      string t (ns, b, e == ns.npos ? e : e - b);

      if (!t.empty ()) os << "};";

      b = e;

      if (b == ns.npos) break;

      b += 2;

    }
    while (true);
  }
}
