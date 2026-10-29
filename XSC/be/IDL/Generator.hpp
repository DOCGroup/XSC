// file      : IDL/Generator.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef IDL_GENERATOR_HPP
#define IDL_GENERATOR_HPP

#include <filesystem>

//@@ SemanticGraphFwd could be useful here.
#include "XSC/SemanticGraph.hpp"
#include "XSC/Options.hpp"

namespace IDL
{
  class Generator
  {
  public:
    void
    generate (XSC::Options const& options,
              XSC::SemanticGraph::Schema&,
              std::filesystem::path const& file);
  };
}


#endif // IDL_GENERATOR_HPP
