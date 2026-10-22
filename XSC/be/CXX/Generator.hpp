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
**/

#ifndef CXX_GENERATOR_HPP
#define CXX_GENERATOR_HPP

#include <filesystem>

//@@ SemanticGraphFwd could be useful here.
//
#include "XSC/SemanticGraph.hpp"
#include "XSC/Options.hpp"

class CXX_Generator
{
public:
  void
  generate (XSC::Options const& options,
            XSC::SemanticGraph::Schema&,
            std::filesystem::path const& file);
};

#endif // CXX_GENERATOR_HPP
