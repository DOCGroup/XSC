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

int
main (int argc, char* argv[])
{
  try
  {
    // Parsing command line options and arguments
    //
    //
    CommandLine cl;

    if (!parse (argc, argv, cl))
    {
      wcerr << "command line syntax error" << endl;
      wcerr << "try " << argv[0] << " --help for usage information" << endl;
      return 1;
    }

    if (cl.get_value ("version", false))
    {
      wcerr << "XML Schema compiler " << version << endl;
      return 0;
    }

    std::string backend (cl.get_value ("backend", ""));

    if (cl.get_value ("help", false) || cl.get_value ("help-html", false))
    {
      CL::Description d (argv[0]);

      if (backend == "cxx")
      {
        CXX_Generator::options (d);
      }
      else if (backend == "idl")
      {
        IDL::Generator::options (d);
      }
      else
      {
        d.add_option (CL::OptionDescription (
                        "backend",
                        "Select backend to use. Supported in this version: \n"
                        "\t\t\tcxx - C++ mapping\n"
                        "\t\t\tidl - CORBA IDL mapping\n",
                        true));

        d.add_option (CL::OptionDescription (
                        "version",
                        "Display version information and exit.",
                        true));

        d.add_option (CL::OptionDescription (
                        "help",
                        "Display usage information and exit. "
                        "Use --backend <name> --help for backend-specific "
                        "information",
                        true));

        /*
          d.add_option (CL::OptionDescription (
          "help-html",
          "Dump usage information in html format and exit.",
          true));
        */
      }


      d.add_argument ("xml schema file");

      if (cl.get_value ("help-html", false)) CL::print_html (std::cerr, d);
      else CL::print_text (std::cerr, d);

      return 0;
    }

    if (backend.empty ()) backend = "cxx";

    if (backend != "cxx" && backend != "idl")
    {
      wcerr << "unknown backend: " << backend.c_str () << endl;
      wcerr << "try " << argv[0] << " --help for backend list" << endl;
      return 1;
    }

    CommandLine::ArgumentsIterator i = cl.arguments_begin ();

    if (i == cl.arguments_end ())
    {
      wcerr << "xsc: error: no input file." << endl;
      return 1;
    }

    // This is a little hack to make XSC signal error conditions
    // by return value. Once I redesign diagnostic in CCF this should
    // be removed.
    //
    ErrorDetector detector (wcerr);

    fs::path tu (*i);

    Parser parser;
    auto_ptr<SemanticGraph::Schema> s (parser.parse (tu));

    if (detector.error ()) return 1;

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
