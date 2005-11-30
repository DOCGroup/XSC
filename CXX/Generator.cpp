// file      : CXX/Generator.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "CXX/Generator.hpp"

#include <set>
#include <cctype>  // std::toupper
#include <ostream>
#include <fstream>

#include <XSC/SemanticGraph.hpp>

#include <CCF/CodeGenerationKit/Regex.hpp>
#include <CCF/CodeGenerationKit/IndentationCxx.hpp>
#include <CCF/CodeGenerationKit/IndentationImplanter.hpp>

#include <CXX/Elements.hpp>

#include <CXX/Forward.hpp>
#include <CXX/Header.hpp>
#include <CXX/Inline.hpp>
#include <CXX/Source.hpp>

#include <CXX/ParserHeader.hpp>
#include <CXX/ParserSource.hpp>

#include <CXX/TypeInfoSource.hpp>

#include <CXX/TraversalHeader.hpp>
#include <CXX/TraversalSource.hpp>

#include <CXX/WriterHeader.hpp>
#include <CXX/WriterSource.hpp>

using std::endl;
using std::wcerr;

using namespace XSC::SemanticGraph;

namespace
{
  // On some platforms std::toupper can be something other than a
  // function with C++ linkage.
  int
  upcase (int c)
  {
    return std::toupper (c);
  }
}

void CXX_Generator::
options (CL::Description& d)
{

  d.add_option (CL::OptionDescription (
                  "cxx-char-type",
                  "type-name",
                  "Generate code using provided character type "
                  "instead of default `wchar_t'.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-generate-writer-types",
                  "Generate code for un-parsing document back to DOM "
                  "which, in turn, can be serialized to a file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-generate-extended-rtti",
                  "Generate extended run-time type information.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-generate-traversal-types",
                  "Generate traversal types that implement visitor-style "
                  "traversal of the graph. Usually useful for performing "
                  "operations on complex recursive and/or dynamically-typed "
                  "(ID/IDREF) graphs.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-generate-inline",
                  "Generate inline implementation for certain functions.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-namespace-regex",
                  "/pattern/replacement/",
                  "Add provided regular expression to the list "
                  "of regular expressions used to translate XML Schema "
                  "namespace names to C++ namespace names. The first "
                  "successful substitution is used. The last specified "
                  "expression is considered first.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-header-suffix",
                  "suffix",
                  "Use provided suffix instead of default `.hpp' "
                  "when constructing the name of the header file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-inline-suffix",
                  "suffix",
                  "Use provided suffix instead of default `.ipp' "
                  "when constructing the name of the inline file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-source-suffix",
                  "suffix",
                  "Use provided suffix instead of default `.cpp' "
                  "when constructing the name of the source file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-header-regex",
                  "/pattern/replacement/",
                  "Use provided regular expression when constructing "
                  "the name of the header file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-inline-regex",
                  "/pattern/replacement/",
                  "Use provided regular expression when constructing "
                  "the name of the inline file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-source-regex",
                  "/pattern/replacement/",
                  "Use provided regular expression when constructing "
                  "the name of the source file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-banner-file",
                  "file-name",
                  "Copy provided banner at the beginning of every generated "
                  "file for which file-specific banner is not provided.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-header-banner-file",
                  "file-name",
                  "Copy provided banner at the beginning of the header file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-inline-banner-file",
                  "file-name",
                  "Copy provided banner at the beginning of the inline file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-source-banner-file",
                  "file-name",
                  "Copy provided banner at the beginning of the source file.",
                  true));

  d.add_option (CL::OptionDescription (
                  "cxx-export-symbol",
                  "symbol",
                  "Export symbol for Win32 DLL export/import control.",
                  true));

  // -- cdr insertion and extraction
  d.add_option (CL::OptionDescription (
                  "cxx-generate-cdr-writer-types",
		  "Generate code for writing types onto "
                  "CDR streams.",
                  true));
  
  d.add_option (CL::OptionDescription (
		  "cxx-generate-cdr-reader-types",
		  "Generate code for reading types from "
                  "CDR streams.",
                  true));

  d.add_option (CL::OptionDescription (
                   "cxx-enable-random-access-sequences",
                   "Generate code that allows random access to "
                   "sequence elements.  Trades off with less efficient"
                   "parsing.",
                   true));
}

void CXX_Generator::
generate (CommandLine const& cl, Schema& schema, fs::path const& file_path)
{
  std::string name (file_path.leaf ());

  std::string hxx_suffix (cl.get_value ("cxx-header-suffix", ".hpp"));
  std::string ixx_suffix (cl.get_value ("cxx-inline-suffix", ".ipp"));
  std::string cxx_suffix (cl.get_value ("cxx-source-suffix", ".cpp"));

  std::string hxx_expr (
    cl.get_value ("cxx-header-regex", "/(\\..+)?$/" + hxx_suffix + "/"));

  std::string ixx_expr (
    cl.get_value ("cxx-inline-regex", "/(\\..+)?$/" + ixx_suffix + "/"));

  std::string cxx_expr (
    cl.get_value ("cxx-source-regex", "/(\\..+)?$/" + cxx_suffix + "/"));


  std::string hxx_name (regex::perl_s (name, hxx_expr));
  std::string ixx_name (regex::perl_s (name, ixx_expr));
  std::string cxx_name (regex::perl_s (name, cxx_expr));

  fs::path hxx_path (hxx_name);
  fs::path ixx_path (ixx_name);
  fs::path cxx_path (cxx_name);

  fs::wofstream hxx (hxx_path, std::ios_base::out);
  fs::wofstream ixx;
  fs::wofstream cxx (cxx_path, std::ios_base::out);

  if (!hxx.is_open ())
  {
    wcerr << hxx_name.c_str () << ": error: unable to open in write mode"
          << endl;
    return;
  }

  bool inline_ (cl.get_value ("cxx-generate-inline", false));

  if (inline_)
  {
    ixx.open (ixx_path, std::ios_base::out);

    if (!ixx.is_open ())
    {
      wcerr << ixx_name.c_str () << ": error: unable to open in write mode"
            << endl;
      return;
    }
  }

  if (!cxx.is_open ())
  {
    wcerr << cxx_name.c_str () << ": error: unable to open in write mode"
          << endl;
    return;
  }

  // Banner.
  //
  {
    using namespace std;

    std::string name (cl.get_value ("cxx-banner-file", ""));
    fs::wifstream banner;

    if (!name.empty ())
    {
      banner.open (name, ios_base::in | ios_base::binary);

      if (!banner.is_open ())
      {
        wcerr << name.c_str () << ": error: unable to open in read mode"
              << endl;
        return;
      }
    }

    // header
    //
    {
      std::string bn (cl.get_value ("cxx-header-banner-file", ""));

      if (!bn.empty ())
      {
        fs::wifstream b (bn, ios_base::in | ios_base::binary);

        if (!b.is_open ())
        {
          wcerr << bn.c_str () << ": error: unable to open in read mode"
                << endl;
          return;
        }

        hxx << b.rdbuf ();
      }
      else if (banner.is_open ())
      {
        hxx << banner.rdbuf ();
        banner.seekg (0, ios_base::beg);
      }
    }

    // inline
    //
    if (inline_)
    {
      std::string bn (cl.get_value ("cxx-inline-banner-file", ""));

      if (!bn.empty ())
      {
        fs::wifstream b (bn, ios_base::in | ios_base::binary);

        if (!b.is_open ())
        {
          wcerr << bn.c_str () << ": error: unable to open in read mode"
                << endl;
          return;
        }

        ixx << b.rdbuf ();
      }
      else if (banner.is_open ())
      {
        ixx << banner.rdbuf ();
        banner.seekg (0, ios_base::beg);
      }
    }

    // source
    //
    {
      std::string bn (cl.get_value ("cxx-source-banner-file", ""));

      if (!bn.empty ())
      {
        fs::wifstream b (bn, ios_base::in | ios_base::binary);

        if (!b.is_open ())
        {
          wcerr << bn.c_str () << ": error: unable to open in read mode"
                << endl;
          return;
        }

        cxx << b.rdbuf ();
      }
      else if (banner.is_open ())
      {
        cxx << banner.rdbuf ();
        banner.seekg (0, ios_base::beg);
      }
    }
  }


  //
  //
  string char_type;

  {
    std::wostringstream ostr;
    ostr << cl.get_value ("cxx-char-type", "wchar_t").c_str ();

    char_type = ostr.str ();
  }


  // Set auto-indentation.
  //
  Indentation::Implanter<Indentation::Cxx, wchar_t> hxx_guard (hxx);
  Indentation::Implanter<Indentation::Cxx, wchar_t> ixx_guard (ixx);
  Indentation::Implanter<Indentation::Cxx, wchar_t> cxx_guard (cxx);

  bool writer (cl.get_value ("cxx-generate-writer-types", false));

  bool traversal (cl.get_value ("cxx-generate-traversal-types", false) ||
                  writer);

  bool rtti (cl.get_value ("cxx-generate-extended-rtti", false) ||
             traversal);

  // Check if cdr reader/writer options are set
  bool cdr_reader (cl.get_value ("cxx-generate-cdr-reader-types", false));
  bool cdr_writer (cl.get_value ("cxx-generate-cdr-writer-types", false));

  // Check about random access sequences
  bool ra_sequences (cl.get_value ("cxx-enable-random-access-sequences", false));
  
  Context::NamespaceMapping nsm;

  // Default mapping.
  //
  nsm.push_back (L"#^.* (.*?/)??"L"(([a-zA-Z_]\\w*)(/[a-zA-Z_]\\w*)*)$#$2#");

  // Custom mappings.
  //
  for (CommandLine::OptionsIterator
         i (cl.options_begin ()), e (cl.options_end ()); i != e; ++i)
  {
    if (i->name () == "cxx-namespace-regex")
    {
      std::wostringstream ostr;
      ostr << i->value ().c_str ();

      nsm.push_back (ostr.str ());
    }
  }

  // Export symbol
  //
  string export_symbol;
  {
    std::string tmp (cl.get_value ("cxx-export-symbol", ""));
    std::wostringstream o;
    o << tmp.c_str ();
    export_symbol = o.str ();
  }

  // HXX
  //
  std::string guard (hxx_name);

  // Split words
  //
  guard = regex::perl_s (guard, "/([a-z])([A-Z])/$1_$2/");

  // Upcase.
  //
  std::transform (guard.begin (), guard.end(), guard.begin (), upcase);

  // Replace '.' with '_'.
  //
  guard = regex::perl_s (guard, "/\\./_/");


  hxx << "#ifndef " << guard.c_str () << endl
      << "#define " << guard.c_str () << endl
      << endl;

  {
    Context ctx (hxx, char_type, export_symbol, nsm);

    // Add information to generate reader/writer types
    ctx.cdr_reader_generation (cdr_reader);
    ctx.cdr_writer_generation (cdr_writer);
    
    // Add additional information to the context:
    ctx.generate_ra_sequences (ra_sequences);
    
    generate_forward (ctx, schema);
    generate_header (ctx, schema, hxx_expr); //@@ move expr to ctx

    generate_parser_header (ctx, schema);

    if (traversal)
    {
      generate_traversal_header (ctx, schema);
    }

    if (writer)
    {
      generate_writer_header (ctx, schema);
    }

  }

  if (inline_)
  {
    hxx << "#include \"" << ixx_name.c_str () << "\"" << endl
        << endl;
  }

  hxx << "#endif // " << guard.c_str () << endl;

  // IXX
  //
  if (inline_)
  {
    Context ctx (ixx, char_type, export_symbol, nsm);

    // Add information to generate reader/writer types
    ctx.cdr_reader_generation (cdr_reader);
    ctx.cdr_writer_generation (cdr_writer);

    // Add additional information to the context:
    ctx.generate_ra_sequences (ra_sequences);
    
    generate_inline (ctx, schema, inline_); //@@ move inline_ to ctx
  }


  // CXX
  //
  cxx << "#include \"" << hxx_name.c_str () << "\"" << endl
      << endl;

  {
    Context ctx (cxx, char_type, export_symbol, nsm);

    // Add information to generate reader/writer types
    ctx.cdr_reader_generation (cdr_reader);
    ctx.cdr_writer_generation (cdr_writer);
    
    // Add additional information to the context:
    ctx.generate_ra_sequences (ra_sequences);
    
    if (!inline_)
    {
      generate_inline (ctx, schema, inline_);
    }

    generate_source (ctx, schema);

    generate_parser_source (ctx, schema);

    if (rtti)
    {
      generate_type_info_source (ctx, schema);
    }

    if (traversal)
    {
      generate_traversal_source (ctx, schema);
    }

    if (writer)
    {
      generate_writer_source (ctx, schema);
    }
  }

}
