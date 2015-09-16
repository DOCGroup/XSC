// file      : IDL/Definition.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef IDL_DEFINITION_HXX
#define IDL_DEFINITION_HXX

#include <string>
#include "Elements.hpp"
#include "XSC/SemanticGraph.hpp"

namespace IDL
{
  void
  generate_definition (Context&,
                       SemanticGraph::Schema&,
                       std::string const& expr);
}

#endif  // IDL_DEFINITION_HXX
