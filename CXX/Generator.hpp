// file      : CXX/Generator.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef CXX_GENERATOR_HPP
#define CXX_GENERATOR_HPP

#include "CCF/CompilerElements/FileSystem.hpp"
#include "CCF/CodeGenerationKit/CommandLine.hpp"
#include "CCF/CodeGenerationKit/CommandLineDescriptor.hpp"

//@@ SemanticGraphFwd could be useful here.
//
#include "XSC/SemanticGraph.hpp"

class CXX_Generator
{
public:
  static void
  options (CL::Description& d);


  void
  generate (CommandLine const& cl,
            XSC::SemanticGraph::Schema&,
            fs::path const& file);
};

#endif // CXX_GENERATOR_HPP
