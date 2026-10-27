#include "Basic_Deployment_Data.hpp"
#include <type_traits>

static_assert (!std::is_convertible_v<::XMLSchema::IDREF<char>,
                                      ::XSCRT::OptionalArg<::XMLSchema::IDREF<char>>>);
static_assert (std::is_constructible_v<::XSCRT::OptionalArg<::XMLSchema::IDREF<char>>,
                                       ::XMLSchema::IDREF<char> const&>);
static_assert (!std::is_constructible_v<::XSCRT::OptionalArg<::XMLSchema::IDREF<char>>,
                                        ::XMLSchema::IDREF<char>&&>);
static_assert (!std::is_constructible_v<::XSCRT::OptionalArg<::XMLSchema::IDREF<char>>,
                                        ::XMLSchema::IDREF<char> const&&>);

int
main ()
{
  Deployment::DataValue sequence;
  Deployment::DataValue sequence_copy (sequence);
  sequence_copy = sequence;

  Deployment::IdRef destination;
  XMLSchema::string<char> href ("urn:destination");
  destination.href (href);
  if (destination.href_if_set () == nullptr)
    return 1;

  Deployment::IdRef source;
  destination = source;

  XMLSchema::IDREF<char> idref ("urn:idref");
  Deployment::IdRef constructed (
    std::nullopt, ::XSCRT::OptionalArg<::XMLSchema::IDREF<char>> (idref));
  if (constructed.idref_if_set () == nullptr)
    return 1;

  Deployment::DataValue::short_container_type shorts;
  shorts.emplace_back (1);
  Deployment::DataValue sequence_constructed (shorts);
  if (sequence_constructed._cxx_short ().size () != 1)
    return 1;

  return destination.href_if_set () != nullptr;
}
