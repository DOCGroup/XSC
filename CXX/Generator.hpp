// file      : CXX/Generator.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef CXX_GENERATOR_HPP
#define CXX_GENERATOR_HPP

#include <CCF/CompilerElements/FileSystem.hpp>

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
