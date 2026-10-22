#include "readonly.hpp"

#include <type_traits>
#include <utility>

namespace
{
  template <typename T, typename = void>
  struct has_name_setter : std::false_type
  {
  };

  template <typename T>
  struct has_name_setter<T, std::void_t<decltype (std::declval<T&> ().name (
      std::declval<::XMLSchema::string<char> const&> ()))>> : std::true_type
  {
  };

  template <typename T, typename = void>
  struct has_optionalValue_presence_accessor : std::false_type
  {
  };

  template <typename T>
  struct has_optionalValue_presence_accessor<T, std::void_t<decltype (
      std::declval<T const&> ().has_optionalValue ())>> : std::true_type
  {
  };

  using ReadOnly = ReadOnlyType;

  static_assert (!std::is_default_constructible_v<ReadOnly>);
  static_assert (!std::is_copy_constructible_v<ReadOnly>);
  static_assert (!std::is_move_constructible_v<ReadOnly>);
  static_assert (!std::is_copy_assignable_v<ReadOnly>);
  static_assert (!std::is_move_assignable_v<ReadOnly>);
  static_assert (!has_name_setter<ReadOnly>::value);
  static_assert (!has_optionalValue_presence_accessor<ReadOnly>::value);
  static_assert (std::is_same_v<decltype (std::declval<ReadOnly&> ().name ()),
                                ::XMLSchema::string<char> const&>);
  static_assert (std::is_same_v<decltype (std::declval<ReadOnly&> ().optionalValue_if_set ()),
                                ::XMLSchema::string<char> const*>);
  static_assert (std::is_same_v<decltype (*std::declval<ReadOnly&> ().begin_item ()),
                                ::XMLSchema::string<char> const&>);
}
