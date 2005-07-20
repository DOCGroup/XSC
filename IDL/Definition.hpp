// file      : IDL/Definition.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#ifndef IDL_DEFINITION_HXX
#define IDL_DEFINITION_HXX

#include <string>

#include <IDL/Elements.hpp>
#include <XSC/SemanticGraph.hpp>

namespace IDL
{
  void
  generate_definition (Context&,
                       SemanticGraph::Schema&,
                       std::string const& expr);
}

#endif  // IDL_DEFINITION_HXX
