#include "toplevel.readonly.hpp"

#include <list>
#include <type_traits>
#include <utility>

namespace
{
  using TopLevel = Deployment::TopLevelPackageDescription;
  using Package = Deployment::PackageConfiguration;
  using Packages = TopLevel::package_container_type;
  using LocalityKind = Deployment::PlanLocalityKind;
  using PlanLocality = Deployment::PlanLocality;

  static_assert (std::is_same_v<Packages, std::list<Package>>);
  static_assert (!std::is_copy_constructible_v<Package>);
  static_assert (!std::is_move_constructible_v<Package>);
  static_assert (std::is_same_v<decltype (*std::declval<Packages const&> ().begin ()),
                                Package const&>);
  static_assert (std::is_same_v<decltype (LocalityKind::SameProcess), LocalityKind::Value>);
  static_assert (std::is_convertible_v<LocalityKind::Value, LocalityKind>);
  static_assert (std::is_same_v<decltype (std::declval<PlanLocality const&> ().constraint () ==
                                           LocalityKind::SameProcess),
                                bool>);
}
