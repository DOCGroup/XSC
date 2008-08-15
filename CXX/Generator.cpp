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
namespace po = boost::program_options;

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
options (po::options_description& d)
{

  d.add_options ()
    ("cxx-char-type", po::value<std::string> ()->default_value ("wchar_t"),
     "Generate code using provided character type instead of wchar_t.")
    ("cxx-generate-writer-types",
     "Generate code for serializing documents back to DOM.")
    ("cxx-generate-extended-rtti",
     "Generate extended run-time type identification")
    ("cxx-generate-traversal-types",
     "Generate traversal types that implement visitor-style "
     "traversal of the graph. Usually useful for performing "
     "operations on complex recursive and/or dynamically-typed "
     "(ID/IDREF) graphs.")
    ("cxx-generate-inline",
     "Generate inline implementation for certain functions.")
    ("cxx-namespace-regex", po::value< std::vector<std::string> > (),
     "Add provided regular expression of the form /pattern/replacement/ to the list "
     "of regular expressions used to translate XML Schema "
     "namespace names to C++ namespace names. The first "
     "successful substitution is used. The last specified "
     "expression is considered first.")
    ("cxx-header-suffix", po::value<std::string> ()->default_value (".hpp"),
     "Use provided suffix instead of default `.hpp' "
     "when constructing the name of the header file.")
    ("cxx-inline-suffix", po::value<std::string> ()->default_value (".ipp"),
     "Use provided suffix instead of default `.ipp' "
     "when constructing the name of the inline file.")
    ("cxx-source-suffix", po::value<std::string> ()->default_value (".cpp"),
     "Use provided suffix instead of default `.cpp' "
     "when constructing the name of the source file.")
    ("cxx-header-regex", po::value<std::string> ()->default_value ("/\\..*$//"),
     "Use provided regular expression of the form /pattern/replacement/ when constructing "
     "the name of the header file.")
    ("cxx-inline-regex", po::value<std::string> ()->default_value ("/\\..*$//"),
     "Use provided regular expression of the form /pattern/replacemsnet when constructing "
     "the name of the inline file.")
    ("cxx-source-regex", po::value<std::string> ()->default_value ("/\\..*$//"),
     "Use provided regular expression of the form /pattern/replacement/ when constructing "
     "the name of the source file.")
    ("cxx-banner-file", po::value<std::string> ()->default_value (""),
     "Copy provided banner at the beginning of every generated "
     "file for which file-specific banner is not provided.")
    ("cxx-header-banner-file", po::value<std::string> ()->default_value (""),
     "Copy provided banner at the beginning of the header file.")
    ("cxx-inline-banner-file", po::value<std::string> ()->default_value (""),
     "Copy provided banner at the beginning of the inline file.")
    ("cxx-source-banner-file", po::value<std::string> ()->default_value (""),
     "Copy provided banner at the beginning of the source file.")
    ("cxx-export-symbol", po::value<std::string> ()->default_value (""),
     "Export symbol for Win32 DLL export/import control.")
    ("cxx-export-header", po::value<std::string> ()->default_value (""),
     "Export header for Win32 DLL export/import control.")
    ("cxx-enable-random-access-sequences",
     "Generate code that allows random access to "
     "sequence elements.  Trades off with less efficient"
     "parsing.")

  // -- cdr insertion and extraction
    ("cxx-generate-cdr-writer-types",
     "Generate code for writing types onto "
     "CDR streams.")
    ("cxx-generate-cdr-reader-types",
     "Generate code for reading types from "
     "CDR streams.")
    ;
}

void CXX_Generator::
generate (po::variables_map const& vm, Schema& schema, fs::path const& file_path)
{
  std::string name (file_path.leaf ());

  std::string hxx_suffix (vm["cxx-header-suffix"].as<std::string> ());
  std::string ixx_suffix (vm["cxx-inline-suffix"].as<std::string> ());
  std::string cxx_suffix (vm["cxx-source-suffix"].as<std::string> ());

  std::string hxx_expr (vm["cxx-header-regex"].as<std::string> ());
  std::string ixx_expr (vm["cxx-inline-regex"].as<std::string> ());
  std::string cxx_expr (vm["cxx-source-regex"].as<std::string> ());


  std::string hxx_name (regex::perl_s (name, hxx_expr) + hxx_suffix);
  std::string ixx_name (regex::perl_s (name, ixx_expr) + ixx_suffix);
  std::string cxx_name (regex::perl_s (name, cxx_expr) + cxx_suffix);

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

  bool inline_ (vm.count ("cxx-generate-inline"));

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

    std::string name (vm["cxx-banner-file"].as<std::string> ());

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
      std::string bn (vm["cxx-header-banner-file"].as<std::string> ());

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
      std::string bn (vm["cxx-inline-banner-file"].as<std::string> ());

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
      std::string bn (vm["cxx-header-banner-file"].as<std::string> ());

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
    ostr << vm["cxx-char-type"].as <std::string> ().c_str ();

    char_type = ostr.str ();
  }


  // Set auto-indentation.
  //
  Indentation::Implanter<Indentation::Cxx, wchar_t> hxx_guard (hxx);
  Indentation::Implanter<Indentation::Cxx, wchar_t> ixx_guard (ixx);
  Indentation::Implanter<Indentation::Cxx, wchar_t> cxx_guard (cxx);

  bool writer (vm.count ("cxx-generate-writer-types"));

  bool traversal (vm.count ("cxx-generate-traversal-types") ||
                  writer);

  bool rtti (vm.count ("cxx-generate-extended-rtti") ||
             traversal);

  // Check if cdr reader/writer options are set
  bool cdr_reader (vm.count ("cxx-generate-cdr-reader-types"));
  bool cdr_writer (vm.count ("cxx-generate-cdr-writer-types"));

  // Check about random access sequences
  bool ra_sequences (vm.count ("cxx-enable-random-access-sequences"));

  NamespaceMapping nsm;

  // Default mapping.
  //
  nsm.push_back (L"#^.* (.*?/)??"L"(([a-zA-Z_]\\w*)(/[a-zA-Z_]\\w*)*)$#$2#");

  if (vm.count ("cxx-namespace-regex"))
    {
      // Custom mappings.
      //
      const std::vector<std::string> &custom_nsm
        (vm["cxx-namespace-regex"].as< std::vector <std::string> > ());

      for (std::vector<std::string>::const_iterator i = custom_nsm.begin ();
           i != custom_nsm.end (); ++i)
        {
          std::wostringstream o;
          o << i->c_str ();
          nsm.push_back (o.str ());
      }
    }

  // Export symbol
  //
  string export_symbol;
  {
    std::wostringstream o;
    o << vm["cxx-export-symbol"].as<std::string> ().c_str ();
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

  // Replace '-' with '_'.
  //
  guard = regex::perl_s (guard, "/\\-/_/");

  hxx << "#ifndef " << guard.c_str () << endl
      << "#define " << guard.c_str () << endl
      << endl;

  // Export header file
  {
    std::string tmp (vm["cxx-export-header"].as<std::string> ());

    if (tmp != "")
      {
        hxx << "#include \"" << tmp.c_str () << '"' << endl;
      }
  }

  {
    ::Context ctx (hxx, char_type, export_symbol, nsm);

    ctx.hxx_expr = hxx_expr.c_str ();
    ctx.hxx_suffix = hxx_suffix.c_str ();

    // Add information to generate reader/writer types
    ctx.cdr_reader_generation (cdr_reader);
    ctx.cdr_writer_generation (cdr_writer);

    // Add additional information to the context:
    ctx.generate_ra_sequences (ra_sequences);

    generate_forward (ctx, schema);
    generate_header (ctx, schema); //@@ move expr to ctx

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
    ::Context ctx (ixx, char_type, export_symbol, nsm);

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
    ::Context ctx (cxx, char_type, export_symbol, nsm);

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
