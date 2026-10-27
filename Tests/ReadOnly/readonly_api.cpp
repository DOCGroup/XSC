#include "readonly.hpp"

#include <list>
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

  template <typename T, typename = void>
  struct has_item_iterators : std::false_type
  {
  };

  template <typename T>
  struct has_item_iterators<T, std::void_t<decltype (std::declval<T const&> ().begin_item ()),
                                           decltype (std::declval<T const&> ().end_item ())>>
    : std::true_type
  {
  };

  template <typename T, typename = void>
  struct has_item_count : std::false_type
  {
  };

  template <typename T>
  struct has_item_count<T, std::void_t<decltype (std::declval<T const&> ().count_item ())>>
    : std::true_type
  {
  };

  using ReadOnly = ReadOnlyType;
  using Derived = DerivedWithoutMembers;
  using SimpleDerived = SimpleContentDerived;
  using ConstOptionalArg = ::XSCRT::OptionalArg<int const>;

  static_assert (std::is_constructible_v<ConstOptionalArg, int const&>);
  static_assert (!std::is_constructible_v<ConstOptionalArg, int const&&>);
  static_assert (!std::is_default_constructible_v<ReadOnly>);
  static_assert (std::is_copy_constructible_v<ReadOnly>);
  static_assert (!std::is_move_constructible_v<ReadOnly>);
  static_assert (!std::is_copy_assignable_v<ReadOnly>);
  static_assert (!std::is_move_assignable_v<ReadOnly>);
  static_assert (!has_name_setter<ReadOnly>::value);
  static_assert (!has_optionalValue_presence_accessor<ReadOnly>::value);
  static_assert (std::is_copy_constructible_v<SimpleDerived>);
  static_assert (std::is_constructible_v<
    SimpleDerived,
    ::XSCRT::OptionalArg<::XMLSchema::string<char>>,
    ::XSCRT::OptionalArg<::XMLSchema::string<char>>>);
  static_assert (!has_item_iterators<ReadOnly>::value);
  static_assert (!has_item_count<ReadOnly>::value);
  static_assert (std::is_constructible_v<Derived>);
  static_assert (std::is_constructible_v<
                 Derived,
                 ::XSCRT::OptionalArg<::XMLSchema::string<char>>>);
  static_assert (std::is_same_v<ReadOnly::item_container_type,
                                std::list<::XMLSchema::string<char>>>);
  static_assert (std::is_same_v<decltype (std::declval<ReadOnly&> ().name ()),
                                ::XMLSchema::string<char> const&>);
  static_assert (std::is_same_v<decltype (std::declval<ReadOnly&> ().optionalValue_if_set ()),
                                ::XMLSchema::string<char> const*>);
  static_assert (std::is_same_v<decltype (std::declval<ReadOnly&> ().item ()),
                                ReadOnly::item_container_type const&>);
}
