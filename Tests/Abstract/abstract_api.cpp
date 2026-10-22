#include "abstract.hpp"

#include <type_traits>

using StudentString = ::XMLSchema::string<ACE_TCHAR>;
using StudentWithoutAdditionalFields =
  ::XSC::Tests::StudentWithoutAdditionalFields;

static_assert (std::is_constructible_v<
  StudentWithoutAdditionalFields,
  StudentString const&,
  StudentString const&,
  StudentString const&>);
static_assert (!std::is_default_constructible_v<
  StudentWithoutAdditionalFields>);
