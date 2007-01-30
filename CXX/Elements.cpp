// file      : CXX/Elements.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <CXX/Elements.hpp>
#include <CCF/CodeGenerationKit/Regex.hpp>
#include <XSC/Traversal.hpp>

#include <sstream>

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
fq_name (SemanticGraph::Nameable& n, string const& nss)
{
  using namespace SemanticGraph;

  string r;

  Scope& s (n.scope ());

  if (!dynamic_cast<Schema*> (&s)) r = fq_name (s, nss);

  if (SemanticGraph::Namespace* ns =
      dynamic_cast<SemanticGraph::Namespace*> (&n))
  {
    string name (ns_name (*ns));

    if (!nss.empty ()) name += L"::" + nss;

    if (!name.empty ())
    {
      r += L"::";
      r += name;
    }
  }
  else
  {
    r += L"::";
    r += id (n.name ());
  }

  return r;
}

namespace
{
  //@@ recode using Fundamental
  //
  struct TypeName : protected virtual Context,
                    Traversal::Type,
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
                    */
                    Traversal::String,
                    Traversal::NormalizedString,
                    Traversal::Token,
                    Traversal::Name,
                    Traversal::NMTOKEN,
                    Traversal::NCName,
                    Traversal::Id,
                    Traversal::IdRef
  {
    TypeName (Context& c, string& r_, string const& nss)
        : Context (c), r (r_), nss_ (nss)
    {
    }

    virtual void
    traverse (SemanticGraph::Type& t)
    {
      r  = fq_name (t, nss_);
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

    */

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

  private:
    string
    schema_ns ()
    {
      return nss_.empty () ? L"::XMLSchema" : L"::XMLSchema::" + nss_;
    }

  private:
    string& r;
    string const& nss_;
  };
}

//@@ In what situations would I produce different name by
//   these two functions?
//
string Context::
type_name (SemanticGraph::Type& t, string const& nss)
{
  // There cannot be a named type inside an anonymous one.
  //
  if (t.named ())
  {
    string r;

    TypeName tn (*this, r, nss);
    tn.dispatch (t);

    return r;
  }

  else return anon_prefix_ + scope + anon_suffix_;
}

string Context::
type_name (SemanticGraph::Instance& i, string const& nss)
{
  SemanticGraph::Type& t (i.type ());

  if (t.named ())
  {
    return type_name (t, nss);
  }
  else
  {
    string r;

    r += anon_prefix_;

    if (t.context ().count ("name"))
    {
      r += t.context ().get<string> ("name");
    }
    else
    {
      //@@ I wonder if this is at all necessary now?
      //

      if (!scope.empty ()) r += scope + L"::";

      //r += fq_name (i.scope ());
      //r += L"::";

      r += id (i.name ());
    }

    r += anon_suffix_;

    return r;
  }
}

namespace
{
  wchar_t* keywords[] = {
    L"asm",
    L"auto",
    L"bool",
    L"break",
    L"case",
    L"catch",
    L"char",
    L"class",
    L"const",
    L"const_cast",
    L"continue",
    L"default",
    L"delete",
    L"do",
    L"double",
    L"dynamic_cast",
    L"else",
    L"enum",
    L"explicit",
    L"export",
    L"extern",
    L"false",
    L"float",
    L"for",
    L"friend",
    L"goto",
    L"if",
    L"inline",
    L"int",
    L"long",
    L"mutable",
    L"namespace",
    L"new",
    L"operator",
    L"private",
    L"protected",
    L"public",
    L"register",
    L"reinterpret_cast",
    L"return",
    L"short",
    L"signed",
    L"sizeof",
    L"static",
    L"static_cast",
    L"struct",
    L"switch",
    L"template",
    L"this",
    L"throw",
    L"true",
    L"try",
    L"typedef",
    L"typeid",
    L"typename",
    L"union",
    L"unsigned",
    L"using",
    L"virtual",
    L"void",
    L"volatile",
    L"wchar_t",
    L"while"
  };
}

string Context::
id (string const& name)
{
  std::size_t const size (sizeof (keywords) / sizeof (wchar_t*));

  if (std::binary_search (keywords, keywords + size, name)) return name + L'_';

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

    if (!t.empty ()) os << "namespace " << id (t) << "{";

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

    if (!t.empty ()) os << "}";

    b = e;

    if (b == ns.npos) break;

    b += 2;

  }
  while (true);
}
