// file      : xsc.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <memory> // std::auto_ptr
#include <iostream>
#include <vector>

#include "boost/program_options.hpp"

#include <CCF/CompilerElements/FileSystem.hpp>

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

namespace po = boost::program_options;

//
// init_basic_commandline_description
//
void init_basic_commandline_description (po::options_description &desc)
{
  desc.add_options ()
    ("help", "Display usage information.  "
     "Use --backend <name> for backend specific information")
    ("version", "Display the current version of XSC")
    ("trace", "Trace parser actions.  Useful for debugging")
    ("backend", po::value<std::string> ()/*->default_value ("")*/,
     "Specify the backend to be used.  Valid options are cxx, idl")
    ("search-path", po::value < std::vector<std::string> > (),
     "Specify search paths for the compiler to use when resolving schema.");
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
    po::options_description basic_desc ("Basic Options");
    po::options_description hidden_desc ("Hidden Options");
    po::options_description cxx_desc ("CXX Backend Options");
    po::options_description idl_desc ("IDL Backend Options");
    
    hidden_desc.add_options ()
      ("input-file", po::value <std::string>  (), "Input schema");

    init_basic_commandline_description (basic_desc);

    CXX_Generator::options (cxx_desc);
    IDL::Generator::options (idl_desc);

    // Parse the command-line options.
    po::variables_map vm;
    
    try
      {
        po::options_description all_options;
        all_options.add (basic_desc);
        all_options.add (hidden_desc);
        all_options.add (cxx_desc);
        all_options.add (idl_desc);
        
        po::positional_options_description p;
        p.add ("input-file", 1);
        
        po::store (po::command_line_parser (argc, argv).
                   options(all_options).positional(p).run (), vm);
        po::notify (vm);
      }
    catch (po::error &ex)
      {
        wcerr << "command line syntax error: " << ex.what () << endl;
        wcerr << "try " << argv[0] << " --help for usage information" << endl;
        return 1;
      }
    catch (...)
      {
        wcerr << "unexpected po exception" << endl;
        return 1;
      }

    // Display the version and then exit.
    if (vm.count("version"))
    {
      wcerr << "XML Schema compiler " << version << endl;
      return 0;
    }

    std::string backend ("");

    if (vm.count ("backend"))
      backend = vm["backend"].as <std::string> ();

    if (vm.count ("help"))
    {
      // @todo: add positional argument.
      if (backend == "cxx")
        basic_desc.add (cxx_desc);
      else if (backend == "idl")
        basic_desc.add (idl_desc);
      
      std::cerr << basic_desc;

      return 0;
    }
    
    // process search paths
    typedef std::vector <std::string> SearchPaths;

    Parser::Paths search_paths;
    search_paths.push_back (fs::path ());
    
    if (vm.count ("search-path"))
      {
        const SearchPaths &search_path_strings (vm["search-path"].as <SearchPaths> ());
        
        for (SearchPaths::const_iterator i = search_path_strings.begin ();
             i != search_path_strings.end ();
             ++i)
          {
            try
              {
                search_paths.push_back (fs::path (*i, fs::native));
              }
            catch (...)
              {
                wcerr << "error: Nonexistant search path supplied:" << i->c_str () << endl;
                return -1;
              }
          }
      }
    
    if (backend != "cxx" && backend != "idl")
      {
        wcerr << "unknown backend: " << backend.c_str () << endl;
        wcerr << "try " << argv[0] << " --help for backend list" << endl;
        return 1;
      }

    const std::string argument (vm["input-file"].as<std::string> ());
    

    if (argument == "")
    {
      wcerr << "error: no input file." << endl;
      return 1;
    }

    // This is a little hack to make XSC signal error conditions
    // by return value. Once I redesign diagnostic in CCF this should
    // be removed.
    //
    ErrorDetector detector (wcerr);

    fs::path tu (argument.c_str ());
    
    Parser parser (vm.count ("trace"), search_paths);
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
      cxx.generate (vm, *s, tu);
    }
    else if (backend == "idl")
    {
      IDL::Generator idl;
      idl.generate (vm, *s, tu);
    }

    return 0;
  }
  catch (boost::bad_any_cast &)
    {
      wcerr << "Bad program option" << endl;
    }
  catch (...)
  {
    wcerr << "caught unknown exception" << endl;
  }

  return 1;
}
