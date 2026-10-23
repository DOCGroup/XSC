#include "test.hpp"

#include <type_traits>
#include <utility>

namespace
{
  using Collision = SequenceAccessorCollision;

  static_assert (std::is_same_v<decltype (std::declval<Collision&> ().value ()),
                                Collision::value_container_type&>);
  static_assert (std::is_same_v<decltype (std::declval<Collision const&> ().value ()),
                                Collision::value_container_type const&>);
  static_assert (std::is_same_v<decltype (std::declval<Collision const&> ().value_values ()),
                                ::XMLSchema::string<char> const&>);
}
