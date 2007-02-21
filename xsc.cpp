// file      : xsc.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <memory> // std::auto_ptr
#include <iostream>

#include <CCF/CompilerElements/FileSystem.hpp>

#include <CCF/CodeGenerationKit/CommandLine.hpp>
#include <CCF/CodeGenerationKit/CommandLineParser.hpp>
#include <CCF/CodeGenerationKit/CommandLineDescriptor.hpp>

#include <XSC/Parser.hpp>

#include <CXX/Validator.hpp>
#include <CXX/Generator.hpp>

#include <IDL/Generator.hpp>

using namespace XSC;

using std::wcerr;
using std::endl;
using std::auto_ptr;

wchar_t const* const version = L"0.0.8";

class ErrorDetector : public std::wstreambuf
{
public:
  ErrorDetector (std::wostream& os)
      : error_ (false),
        prev_ (*(os.rdbuf ())),
        os_ (os)
  {
    os_.rdbuf (this);
  }

  ~ErrorDetector ()
  {
    os_.rdbuf (&prev_);
  }

  virtual int_type
  overflow (int_type c)
  {
    error_ = true;
    return prev_.sputc (c);
  }

  virtual int
  sync ()
  {
    return prev_.pubsync ();
  }

  bool
  error () const throw ()
  {
    return error_;
  }

  void
  reset ()
  {
    error_ = false;
  }

private:
  bool error_;
  std::wstreambuf& prev_;
  std::wostream& os_;
};

//
// init_basic_commandline_description
//
int init_basic_commandline_description (CL::Description & d)
{
  // We have to initialize the decription with the minimal amount
  // of options, e.g., help options, before we can do any parsing.
  d.add_option (CL::OptionDescription (
                "help",
                "Display usage information.\n\t\tUse --backend <name> "
                "--help for backend-specific information.",
                CL::OptionType::flag,
                true));

  d.add_option (CL::OptionDescription (
                "help-html",
                "Display usage information in HTML format.\n\t\tUse "
                "--backend <name> --help for backend-specific information.",
                CL::OptionType::flag,
                true));

  d.add_option (CL::OptionDescription (
                "version",
                "Display the version of XSC\n",
                CL::OptionType::flag,
                true));

  d.add_option (CL::OptionDescription (
                "trace",
                "Trace parser actions. Useful for debugging.\n",
                CL::OptionType::flag,
                true));

  d.add_option (CL::OptionDescription (
                "backend",
                "\tSelect backend to use.",
                "\tcxx - C++ mapping\n"
                "\t\t\tidl - CORBA IDL mapping\n",
                CL::OptionType::value,
                false));
  
  d.add_option (CL::OptionDescription ("search-path",
                                       "\tAdd to search path",
                                       "\tAdd a path to the list of those searched by XSC when opening an include.",
                                       CL::OptionType::value,
                                       false));
  return 0;
}

//
// main
//
int main (int argc, char* argv[])
{
  try
  {
    // We are going to play a little trick here and have a description
    // of all the options that we pass to the parse (...) method. We
    // are also going to have one of only the basic options that we
    // use for displaying things. This is necessary because the new
    // implemenation of parse (...) thinks everthing thats not in the
    // description is a "argument" and not a "option" if it is found
    // on argv[].
    CommandLine cl;
    CL::Description basic_desc (argv[0]),
                    complete_desc (argv[0]);

    init_basic_commandline_description (basic_desc);
    init_basic_commandline_description (complete_desc);

    CXX_Generator::options (complete_desc);
    IDL::Generator::options (complete_desc);

    // Parse the command-line options.
    if (!parse (argc, argv, complete_desc, cl))
    {
      wcerr << "command line syntax error" << endl;
      wcerr << "try " << argv[0] << " --help for usage information" << endl;
      return 1;
    }

    // Display the version and then exit.
    if (cl.get_value ("version", false))
    {
      wcerr << "XML Schema compiler " << version << endl;
      return 0;
    }

    std::string backend (cl.get_value ("backend", ""));

    if (cl.get_value ("help", false) ||
        cl.get_value ("help-html", false))
    {
      // Fill in the basic description with the options for the
      // specific backend.

      if (backend == "cxx")
        CXX_Generator::options (basic_desc);
      else if (backend == "idl")
        IDL::Generator::options (basic_desc);

      // Add the argument to the command line options.
      basic_desc.add_argument ("xml schema file");

      // Write the output in the correct format.
      if (cl.get_value ("help-html", false))
        CL::print_html (std::cerr, basic_desc);
      else
        CL::print_text (std::cerr, basic_desc);

      return 0;
    }
    
    // process search paths
    std::vector <std::string> search_path_strings;
    cl.get_all_values ("search-path",
                       search_path_strings);
    
    Parser::Paths search_paths;
    search_paths.push_back (fs::path ());
    
    for (std::vector <std::string>::const_iterator i = search_path_strings.begin ();
         i != search_path_strings.end ();
         ++i)
      {
        try
          {
            search_paths.push_back (fs::path (*i, fs::native));
          }
        catch (...)
          {
            wcerr << "error: Nonexistaqnt search path supplied:" << i->c_str () << endl;
            return -1;
          }
      }
    
    // Set the default backend to CXX.
    if (backend.empty ())
      backend = "cxx";

    if (backend != "cxx" && backend != "idl")
    {
      wcerr << "unknown backend: " << backend.c_str () << endl;
      wcerr << "try " << argv[0] << " --help for backend list" << endl;
      return 1;
    }

    CommandLine::ArgumentsIterator i = cl.arguments_begin ();

    if (i == cl.arguments_end ())
    {
      wcerr << "error: no input file." << endl;
      return 1;
    }

    // This is a little hack to make XSC signal error conditions
    // by return value. Once I redesign diagnostic in CCF this should
    // be removed.
    //
    ErrorDetector detector (wcerr);

    fs::path tu (*i);
    
    Parser parser (cl.get_value ("trace", false), search_paths);
    auto_ptr <SemanticGraph::Schema> s (parser.parse (tu));

    if (detector.error ())
      {
        wcerr << "Parsing encountered an error.\n";
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
      
      CXX_Generator cxx;
      cxx.generate (cl, *s, tu);
    }
    else if (backend == "idl")
    {
      IDL::Generator idl;
      idl.generate (cl, *s, tu);
    }

    return 0;
  }
  catch (...)
  {
    wcerr << "caught unknown exception" << endl;
  }

  return 1;
}
