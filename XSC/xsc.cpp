// -*- C++ -*-

//=============================================================================
/**
 * @file    xsc.cpp
 *
 * XSC main source file.  Takes in command line arguments and an XML Schema and
 * outputs a C++ class hierarchy that will allow for parsing and interaction of XML
 * documents that match the supplied schema.
 *
 * XSC Basic Run :
 * 1. Creates and stores program options.
 * 2. Parses Command Line Arguments.
 * 3. Creates parser and schema (builds semantic graph).
 * 4. Depending on backend cmd line options, creates appropriate
 *    Generator object.
 * 5. Generates output files.
 * 6. Program will return an error under the following conditions:
 *    (a)An input file is not supplied (*.xsd)
 *    (b)An incorrect program option is entered at the command line.
 *    (c)An error occurs during the parsing of the supplied schema.
 *
 * @author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
 */
//=============================================================================

#include <filesystem>
#include <memory>
#include <iostream>
#include <string_view>
#include <vector>

#include "ace/Get_Opt.h"
#include "XSC/Options.hpp"
#include "XSC/Parser.hpp"
#include "XSC/be/CXX/Validator.hpp"
#include "XSC/be/CXX/Generator.hpp"
#include "XSC/be/IDL/Generator.hpp"

using namespace XSC;

wchar_t const* const version = L"0.0.8";

 /**
 * @class ErrorDetector
 *
 * @brief A class that is used to make XSC signal error conditions
 *  by return value.
 */

class ErrorDetector : public std::wstreambuf
{
public:
  ErrorDetector (std::wostream& os)
      : prev_ (*(os.rdbuf ())),
        os_ (os)
  {
    os_.rdbuf (this);
  }

  ~ErrorDetector () override
  {
    os_.rdbuf (std::addressof (prev_));
  }

  int_type
  overflow (int_type c) override
  {
    error_ = true;
    return prev_.sputc (c);
  }

  int
  sync () override
  {
    return prev_.pubsync ();
  }

  bool
  error () const noexcept
  {
    return error_;
  }

  void
  reset ()
  {
    error_ = false;
  }

private:
  ErrorDetector (ErrorDetector const&) = delete;
  ErrorDetector& operator= (ErrorDetector const&) = delete;
  ErrorDetector (ErrorDetector&&) = delete;
  ErrorDetector& operator= (ErrorDetector&&) = delete;

  bool error_ {};
  std::wstreambuf& prev_;
  std::wostream& os_;
};

namespace
{
  struct Option
  {
    char const* name;
    ACE_Get_Opt::OPTION_ARG_MODE argument;
    char const* description;
  };

  std::vector<Option> const options {
    {"help", ACE_Get_Opt::NO_ARG, "Display usage information."},
    {"version", ACE_Get_Opt::NO_ARG, "Display the current version of XSC."},
    {"trace", ACE_Get_Opt::NO_ARG, "Trace parser actions."},
    {"backend", ACE_Get_Opt::ARG_REQUIRED, "Backend: cxx or idl."},
    {"search-path", ACE_Get_Opt::ARG_REQUIRED, "Schema search path (repeatable)."},
    {"cxx-char-type", ACE_Get_Opt::ARG_REQUIRED, "Generated C++ character type."},
    {"cxx-orb", ACE_Get_Opt::ARG_REQUIRED, "ORB integration: tao or taox11."},
    {"cxx-generate-writer-types", ACE_Get_Opt::NO_ARG, "Generate DOM writer types."},
    {"cxx-generate-extended-rtti", ACE_Get_Opt::NO_ARG, "Generate extended RTTI."},
    {"cxx-generate-traversal-types", ACE_Get_Opt::NO_ARG, "Generate traversal types."},
    {"cxx-generate-inline", ACE_Get_Opt::NO_ARG, "Generate inline implementations."},
    {"cxx-namespace-regex", ACE_Get_Opt::ARG_REQUIRED, "Namespace mapping regex (repeatable)."},
    {"cxx-header-suffix", ACE_Get_Opt::ARG_REQUIRED, "Generated header suffix."},
    {"cxx-inline-suffix", ACE_Get_Opt::ARG_REQUIRED, "Generated inline suffix."},
    {"cxx-source-suffix", ACE_Get_Opt::ARG_REQUIRED, "Generated source suffix."},
    {"cxx-header-regex", ACE_Get_Opt::ARG_REQUIRED, "Generated header filename regex."},
    {"cxx-inline-regex", ACE_Get_Opt::ARG_REQUIRED, "Generated inline filename regex."},
    {"cxx-source-regex", ACE_Get_Opt::ARG_REQUIRED, "Generated source filename regex."},
    {"cxx-header-guard", ACE_Get_Opt::ARG_REQUIRED, "Generated header guard."},
    {"cxx-banner-file", ACE_Get_Opt::ARG_REQUIRED, "Common generated-file banner."},
    {"cxx-header-banner-file", ACE_Get_Opt::ARG_REQUIRED, "Generated header banner."},
    {"cxx-inline-banner-file", ACE_Get_Opt::ARG_REQUIRED, "Generated inline banner."},
    {"cxx-source-banner-file", ACE_Get_Opt::ARG_REQUIRED, "Generated source banner."},
    {"cxx-export-symbol", ACE_Get_Opt::ARG_REQUIRED, "DLL export symbol."},
    {"cxx-export-header", ACE_Get_Opt::ARG_REQUIRED, "DLL export header."},
    {"cxx-enable-random-access-sequences", ACE_Get_Opt::NO_ARG, "Generate random-access sequences."},
    {"cxx-generate-cdr-writer-types", ACE_Get_Opt::NO_ARG, "Generate CDR writer types."},
    {"cxx-generate-cdr-reader-types", ACE_Get_Opt::NO_ARG, "Generate CDR reader types."},
    {"cxx-cpp03", ACE_Get_Opt::NO_ARG, "Generate C++03-compatible code."},
    {"cxx-cpp11", ACE_Get_Opt::NO_ARG, "Generate C++11 code."},
    {"cxx-cpp17", ACE_Get_Opt::NO_ARG, "Generate C++17 code."},
    {"idl-char-type", ACE_Get_Opt::ARG_REQUIRED, "Generated IDL character type."},
    {"idl-namespace-regex", ACE_Get_Opt::ARG_REQUIRED, "IDL namespace mapping regex (repeatable)."},
    {"idl-file-suffix", ACE_Get_Opt::ARG_REQUIRED, "Generated IDL suffix."},
    {"idl-file-regex", ACE_Get_Opt::ARG_REQUIRED, "Generated IDL filename regex."},
    {"idl-banner-file", ACE_Get_Opt::ARG_REQUIRED, "Generated IDL banner."}
  };

  bool
  repeatable (std::string_view name)
  {
    return name == "search-path" || name == "cxx-namespace-regex" ||
           name == "idl-namespace-regex";
  }

  void
  print_help (std::string_view backend)
  {
    std::cout << "Usage: xsc --backend <cxx|idl> [options] schema.xsd\n\nOptions:\n";

    for (Option const& option : options)
      {
        std::string_view const name {option.name};
        if (option.name == std::string_view {"help"} ||
            option.name == std::string_view {"version"} ||
            option.name == std::string_view {"trace"} ||
            option.name == std::string_view {"backend"} ||
            option.name == std::string_view {"search-path"} ||
            (!backend.empty () && name.size () >= backend.size () &&
             name.substr (0, backend.size ()) == backend))
          {
            std::cout << "  --" << option.name;
            if (option.argument == ACE_Get_Opt::ARG_REQUIRED)
              std::cout << " <value>";
            std::cout << "\n      " << option.description << '\n';
          }
      }
  }
}


//
// main
//
int main (int argc, char* argv[])
{
  try
  {
    XSC::Options command_line;
    ACE_Get_Opt get_opt (argc, argv, "", 1, 0, ACE_Get_Opt::PERMUTE_ARGS);

    for (Option const& option : options)
      get_opt.long_option (option.name, option.argument);

    for (int option {}; (option = get_opt ()) != EOF;)
      {
        if (option != 0)
          {
            std::wcerr << "command line syntax error near " << get_opt.last_option () << std::endl;
            std::wcerr << "try " << argv[0] << " --help for usage information" << std::endl;
            return 1;
          }

        std::string const name {get_opt.long_option ()};
        char const* const argument {get_opt.opt_arg ()};

        if (argument == nullptr)
          command_line.add (name);
        else if (repeatable (name))
          command_line.add (name, argument);
        else
          command_line.set (name, argument);
      }

    std::string backend;
    if (command_line.count ("backend"))
      backend = command_line.value ("backend");

    if (command_line.count ("version"))
      {
        std::wcerr << "XML Schema compiler " << version << std::endl;
        return 0;
      }

    if (command_line.count ("help"))
      {
        print_help (backend);
        return 0;
      }

    if (argc - get_opt.opt_ind () != 1)
      {
        std::wcerr << "error: exactly one input schema is required." << std::endl;
        return 1;
      }

    command_line.set ("input-file", get_opt.argv ()[get_opt.opt_ind ()]);
    command_line.set ("cxx-char-type", command_line.count ("cxx-char-type") ? command_line.value ("cxx-char-type") : "ACE_TCHAR");
    command_line.set ("cxx-orb", command_line.count ("cxx-orb") ? command_line.value ("cxx-orb") : "tao");
    command_line.set ("cxx-header-suffix", command_line.count ("cxx-header-suffix") ? command_line.value ("cxx-header-suffix") : ".hpp");
    command_line.set ("cxx-inline-suffix", command_line.count ("cxx-inline-suffix") ? command_line.value ("cxx-inline-suffix") : ".ipp");
    command_line.set ("cxx-source-suffix", command_line.count ("cxx-source-suffix") ? command_line.value ("cxx-source-suffix") : ".cpp");
    command_line.set ("cxx-header-regex", command_line.count ("cxx-header-regex") ? command_line.value ("cxx-header-regex") : "/\\..*$//");
    command_line.set ("cxx-inline-regex", command_line.count ("cxx-inline-regex") ? command_line.value ("cxx-inline-regex") : "/\\..*$//");
    command_line.set ("cxx-source-regex", command_line.count ("cxx-source-regex") ? command_line.value ("cxx-source-regex") : "/\\..*$//");
    command_line.set ("cxx-header-guard", command_line.count ("cxx-header-guard") ? command_line.value ("cxx-header-guard") : "");
    command_line.set ("cxx-banner-file", command_line.count ("cxx-banner-file") ? command_line.value ("cxx-banner-file") : "");
    command_line.set ("cxx-header-banner-file", command_line.count ("cxx-header-banner-file") ? command_line.value ("cxx-header-banner-file") : "");
    command_line.set ("cxx-inline-banner-file", command_line.count ("cxx-inline-banner-file") ? command_line.value ("cxx-inline-banner-file") : "");
    command_line.set ("cxx-source-banner-file", command_line.count ("cxx-source-banner-file") ? command_line.value ("cxx-source-banner-file") : "");
    command_line.set ("cxx-export-symbol", command_line.count ("cxx-export-symbol") ? command_line.value ("cxx-export-symbol") : "");
    command_line.set ("cxx-export-header", command_line.count ("cxx-export-header") ? command_line.value ("cxx-export-header") : "");
    command_line.set ("idl-char-type", command_line.count ("idl-char-type") ? command_line.value ("idl-char-type") : "wchar");
    command_line.set ("idl-file-suffix", command_line.count ("idl-file-suffix") ? command_line.value ("idl-file-suffix") : ".idl");
    command_line.set ("idl-file-regex", command_line.count ("idl-file-regex") ? command_line.value ("idl-file-regex") : "/\\..*$//");
    command_line.set ("idl-banner-file", command_line.count ("idl-banner-file") ? command_line.value ("idl-banner-file") : "");

    // process search paths
    using SearchPaths = std::vector<std::string>;

    Parser::Paths search_paths;
    search_paths.push_back (std::filesystem::path ());

    if (command_line.count ("search-path"))
      {
        SearchPaths const& search_path_strings {command_line.values ("search-path")};

        for (std::string const& search_path : search_path_strings)
          {
            try
              {
                search_paths.push_back (std::filesystem::path (search_path));
              }
            catch (...)
              {
                std::wcerr << "error: Nonexistant search path supplied:" << search_path.c_str () << std::endl;
                return -1;
              }
          }
      }

    if (backend != "cxx" && backend != "idl")
      {
        std::wcerr << "unknown backend: " << backend.c_str () << std::endl;
        std::wcerr << "try " << argv[0] << " --help for backend list" << std::endl;
        return 1;
      }

    std::string const argument {command_line.value ("input-file")};


    if (argument == "")
    {
      std::wcerr << "error: no input file." << std::endl;
      return 1;
    }

    // This is a little hack to make XSC signal error conditions
    // by return value. Once I redesign diagnostic in CCF this should
    // be removed.
    //
    ErrorDetector detector (std::wcerr);

    //"tu" is set to the schema path provided in the command line.
    std::filesystem::path tu (argument.c_str ());

    Parser parser (command_line.count ("trace"), search_paths);

    //File is parsed and loaded into the Semantic Graph::Schema (s)
    std::unique_ptr<SemanticGraph::Schema> s (parser.parse (tu));

    if (detector.error ())
      {
        std::wcerr << "Parsing encountered an error.\n";
        return 1;
      }

    if (backend == "cxx")
    {
      // Let C++ mapping evaluate the graph for possibility of generating
      // something useful.
      //
      if (!CXX::validate (*s))
      {
        return 1;
      }

      //Create a CXX Generator object.
      CXX_Generator cxx;

      //Generate the output based on the Schema (created from the
      //parsing of the provided schema)
      cxx.generate (command_line, *s, tu);
    }
    else if (backend == "idl")
    {
      //Create an IDL Generator object
      IDL::Generator idl;

      //Generate the output based on the Schema (created from the
      //parsing of the provided schema)
      idl.generate (command_line, *s, tu);
    }

    return 0;
  }
  catch (...)
  {
    std::wcerr << "caught unknown exception" << std::endl;
  }

  return 1;
}
