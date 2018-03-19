// file      : CCF/IDL3/SemanticAction/Impl/Uses.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef CCF_IDL3_SEMANTIC_ACTION_IMPL_USES_HPP
#define CCF_IDL3_SEMANTIC_ACTION_IMPL_USES_HPP

#include "CCF/IDL2/SemanticGraph/Interface.hpp"
#include "CCF/IDL3/SemanticAction/Uses.hpp"
#include "CCF/IDL3/SemanticAction/Impl/Elements.hpp"

namespace CCF
{
  namespace IDL3
  {
    namespace SemanticAction
    {
      namespace Impl
      {
        struct Uses : SemanticAction::Uses, Base
        {
          explicit Uses (Context& c);

          virtual void
          multiple ();

          virtual void
          type (IdentifierPtr const& id);

          virtual void
          name (SimpleIdentifierPtr const& id);

        private:
          bool multiple_;
          SemanticGraph::Interface* type_;
        };
      }
    }
  }
}

#endif  // CCF_IDL3_SEMANTIC_ACTION_IMPL_USES_HPP
