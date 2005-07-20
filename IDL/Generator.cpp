// file      : IDL/Generator.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <IDL/Generator.hpp>

#include <set>
#include <cctype>  // std::toupper
#include <ostream>
#include <fstream>

#include <XSC/SemanticGraph.hpp>

#include <CCF/CodeGenerationKit/Regex.hpp>
#include <CCF/CodeGenerationKit/IndentationIDL.hpp>
#include <CCF/CodeGenerationKit/IndentationImplanter.hpp>

#include <IDL/Elements.hpp>

#include <IDL/Declaration.hpp>
#include <IDL/Definition.hpp>

using std::endl;
using std::wcerr;

using namespace XSC::SemanticGraph;

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
  options (CL::Description& d)
  {

    d.add_option (CL::OptionDescription (
                    "idl-char-type",
                    "type-name",
                    "Generate code using provided character type "
                    "instead of default `wchar'.",
                    true));

    d.add_option (CL::OptionDescription (
                    "idl-namespace-regex",
                    "/pattern/replacement/",
                    "Add provided regular expression to the list "
                    "of regular expressions used to translate XML Schema "
                    "namespace names to IDL module names. The first successful "
                    "substitution is used. The last specified expression "
                    "is considered first.",
                    true));

    d.add_option (CL::OptionDescription (
                    "idl-file-suffix",
                    "suffix",
                    "Use provided suffix instead of default `.idl' "
                    "when constructing the name of the IDL file.",
                    true));

    d.add_option (CL::OptionDescription (
                    "idl-file-regex",
                    "/pattern/replacement/",
                    "Use provided regular expression when constructing "
                    "the name of the IDL file.",
                    true));

    d.add_option (CL::OptionDescription (
                    "idl-banner-file",
                    "file-name",
                    "Copy provided banner at the beginning of every generated "
                    "file for which file-specific banner is not provided.",
                    true));
  }

  void Generator::
  generate (CommandLine const& cl, Schema& schema, fs::path const& in_path)
  {
    std::string in_name (in_path.leaf ());

    std::string suffix (cl.get_value ("idl-file-suffix", ".idl"));

    std::string expr (
      cl.get_value ("idl-file-regex", "/(\\..+)?$/" + suffix + "/"));

    std::string out_name (regex::perl_s (in_name, expr));

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

      std::string name (cl.get_value ("idl-banner-file", ""));
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
      ostr << cl.get_value ("idl-char-type", "wchar").c_str ();

      char_type = ostr.str ();
    }

    // Set auto-indentation.
    //
    Indentation::Implanter<Indentation::IDL, wchar_t> idl_guard (idl);


    Context::NamespaceMapping nsm;


    // Default mapping.
    //
    nsm.push_back (L"#^.* (.*?/)??"L"(([a-zA-Z_]\\w*)(/[a-zA-Z_]\\w*)*)$#$2#");

    // Custom mappings.
    //
    for (CommandLine::OptionsIterator
           i (cl.options_begin ()), e (cl.options_end ()); i != e; ++i)
    {
      if (i->name () == "idl-namespace-regex")
      {
        std::wostringstream ostr;
        ostr << i->value ().c_str ();

        nsm.push_back (ostr.str ());
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
