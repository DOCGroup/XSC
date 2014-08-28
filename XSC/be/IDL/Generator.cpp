// file      : IDL/Generator.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "Generator.hpp"
#include "Elements.hpp"
#include "Declaration.hpp"
#include "Definition.hpp"
#include "XSC/SemanticGraph.hpp"
#include "CCF/CodeGenerationKit/Regex.hpp"
#include "CCF/CodeGenerationKit/IndentationIDL.hpp"
#include "CCF/CodeGenerationKit/IndentationImplanter.hpp"
#include <set>
#include <cctype>  // std::toupper
#include <ostream>
#include <fstream>

using std::endl;
using std::wcerr;

using namespace XSC::SemanticGraph;

namespace po = boost::program_options;

namespace IDL
{
  namespace
  {
    // On some platforms std::toupper can be something other than a
    // function with C++ linkage.
    //
    int
    upcase (int c)
    {
      return std::toupper (c);
    }
  }

  void Generator::
  options (po::options_description& d)
  {
    d.add_options ()
      ("idl-char-type", po::value<std::string> ()->default_value ("wchar"),
       "Generate code using provided character type "
       "instead of default `wchar'.")

      ("idl-namespace-regex", po::value< std::vector<std::string> > (),
       "Add provided regular expression of the form /pattern/replacement/ to the list "
       "of regular expressions used to translate XML Schema "
       "namespace names to IDL module names. The first successful "
       "substitution is used. The last specified expression "
       "is considered first.")

     ("idl-file-suffix", po::value<std::string> ()->default_value (".idl"),
      "Use provided suffix instead of default `.idl' "
      "when constructing the name of the IDL file.")

     ("idl-file-regex", po::value<std::string> ()->default_value ("/\\..*$//"),
      "Use provided regular expression in the format /pattern/replacement/  when constructing "
      "the name of the IDL file.")

     ("idl-banner-file", po::value<std::string> ()->default_value (""),
       "Copy provided banner at the beginning of every generated "
       "file for which file-specific banner is not provided.")
      ;
  }

  void Generator::
  generate (po::variables_map const& vm, Schema& schema, fs::path const& in_path)
  {
#if BOOST_FILESYSTEM_VERSION == 2
    std::string in_name (in_path.leaf ());
#else
    std::string in_name (in_path.filename ().string ());
#endif

    std::string suffix (vm["idl-file-suffix"].as<std::string> ());

    std::string expr (vm["idl-file-regex"].as<std::string> ());
    //, "/(\\..+)?$/" + suffix + "/"));

    std::string out_name (regex::perl_s (in_name, expr));
    out_name += suffix;

    fs::path out_path (out_name);

    fs::wofstream idl (out_path, std::ios_base::out);

    if (!idl.is_open ())
    {
      wcerr << out_name.c_str () << ": error: unable to open in write mode"
            << endl;
      return;
    }

    // Banner.
    //
    {
      using namespace std;

      std::string name (vm["idl-banner-file"].as<std::string> ());
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

        idl << banner.rdbuf ();
      }
    }


    //
    //
    string char_type;

    {
      std::wostringstream ostr;
      ostr << vm["idl-char-type"].as<std::string> ().c_str ();

      char_type = ostr.str ();
    }

    // Set auto-indentation.
    //
    Indentation::Implanter<Indentation::IDL, wchar_t> idl_guard (idl);


    Context::NamespaceMapping nsm;

    // Default mapping.
    nsm.push_back (L"#^.* (.*?/)??" L"(([a-zA-Z_]\\w*)(/[a-zA-Z_]\\w*)*)$#$2#");

    // Custom mappings.
    //
    if (vm.count ("idl-namespace-regex"))
      {
        const std::vector<std::string> &custom_nsm
          (vm["idl-namespace-regex"].as< std::vector <std::string> > ());

        for (std::vector<std::string>::const_iterator i = custom_nsm.begin ();
            i != custom_nsm.end (); ++i)
          {
            std::wostringstream o;
            o << i->c_str ();
            nsm.push_back (o.str ());
          }
      }

    std::string guard (out_name);

    // Split words
    //
    guard = regex::perl_s (guard, "/([a-z])([A-Z])/$1_$2/");

    // Upcase.
    //
    std::transform (guard.begin (), guard.end(), guard.begin (), upcase);

    // Replace '.' with '_'.
    //
    guard = regex::perl_s (guard, "/\\./_/");


    idl << "#ifndef " << guard.c_str () << endl
        << "#define " << guard.c_str () << endl
        << endl;

    {
      Context ctx (idl, false, char_type, nsm);

      generate_declaration (ctx, schema);
      generate_definition (ctx, schema, expr); //@@ move expr to context
    }

    idl << "#endif // " << guard.c_str () << endl;
  }
}
