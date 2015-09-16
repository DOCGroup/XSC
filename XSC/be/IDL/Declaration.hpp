// file      : IDL/Declaration.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef IDL_DECLARATION_HXX
#define IDL_DECLARATION_HXX

#include "Elements.hpp"
#include "XSC/SemanticGraph.hpp"

namespace IDL
{
  void
  generate_declaration (Context&, SemanticGraph::Schema&);
}

#endif  // IDL_DECLARATION_HXX
