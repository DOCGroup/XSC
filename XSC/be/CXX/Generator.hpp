// -*- C++ -*-

//=============================================================================
/**
 * @file    Generator.hpp
 *
 * CXX Backend Generator.hpp.  This file generates the output
 *   C++ files and class structure dictated by the DOMDocument 
 *   parsed from the ".xsd" passed into xsc.exe.
 * 
 *    CXX_Generator::options : Initializes the program options
 *    for the cxx generator.
 * 
 *    CXX_Generator::generate : Generates the output C++ files 
 *    and class structures.
 * 
 * XSC Basic Run step 5:
 * (a) Read in program option variables.
 * (b) Create file handlers.
 * (c) Read in banner (header, inline, and source file banners).
 * (d) Set char_type from the command line (default is ACE_TCHAR).
 * (e) Instantiate Context from the program options.
 * (f) Generate C++ files based on the output names and suffixes from
 *     the command line.
 * 
 * file      : CXX/Generator.hpp
 * author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
 * cvs-id    : $Id$
**/

#ifndef CXX_GENERATOR_HPP
#define CXX_GENERATOR_HPP

#include "CCF/CompilerElements/FileSystem.hpp"

//@@ SemanticGraphFwd could be useful here.
//
#include "XSC/SemanticGraph.hpp"

#include "boost/program_options.hpp"

class CXX_Generator
{
public:
  static void
  options (boost::program_options::options_description &d);


  void
  generate (boost::program_options::variables_map const& vm,
            XSC::SemanticGraph::Schema&,
            fs::path const& file);
};

#endif // CXX_GENERATOR_HPP
