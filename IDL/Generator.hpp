// file      : IDL/Generator.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef IDL_GENERATOR_HPP
#define IDL_GENERATOR_HPP

#include "CCF/CompilerElements/FileSystem.hpp"
#include "CCF/CodeGenerationKit/CommandLine.hpp"
#include "CCF/CodeGenerationKit/CommandLineDescriptor.hpp"

//@@ SemanticGraphFwd could be useful here.
//
#include "XSC/SemanticGraph.hpp"

namespace IDL
{
  class Generator
  {
  public:
    static void
    options (CL::Description& d);


    void
    generate (CommandLine const& cl,
              XSC::SemanticGraph::Schema&,
              fs::path const& file);
  };
}


#endif // IDL_GENERATOR_HPP
