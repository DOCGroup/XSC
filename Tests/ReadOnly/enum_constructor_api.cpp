#include "enum_constructor.hpp"

#include <type_traits>

static_assert (std::is_constructible_v<ReadOnlyKindWithOptionalAttribute,
                                       ReadOnlyKind const&,
                                       ::XSCRT::OptionalArg<::XMLSchema::string<char>>>);
