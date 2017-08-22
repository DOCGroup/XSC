// file      : IDL/Generator.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef IDL_GENERATOR_HPP
#define IDL_GENERATOR_HPP

#include <CCF/CompilerElements/FileSystem.hpp>

//@@ SemanticGraphFwd could be useful here.
//
#include "XSC/SemanticGraph.hpp"

#include "boost/program_options.hpp"

namespace IDL
{
  class Generator
  {
  public:
    static void
    options (boost::program_options::options_description &d);


    void
    generate (boost::program_options::variables_map const& vm,
              XSC::SemanticGraph::Schema&,
              fs::path const& file);
  };
}


#endif // IDL_GENERATOR_HPP
