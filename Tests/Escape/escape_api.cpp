#include "test.hpp"

#include <type_traits>
#include <utility>

namespace
{
  using EscapedClass = test::_cxx_class;

  static_assert (std::is_same_v<decltype (std::declval<EscapedClass const&> ()._cxx_int ()),
                                ::XMLSchema::int_ const&>);
  static_assert (std::is_same_v<decltype (std::declval<EscapedClass const&> ()._cxx_long_if_set ()),
                                ::XMLSchema::long_ const*>);
  static_assert (std::is_same_v<decltype (std::declval<EscapedClass const&> ()._cxx_double ()),
                                EscapedClass::double_container_type const&>);
  static_assert (std::is_same_v<decltype (test::reader::_cxx_switch (nullptr)),
                                EscapedClass>);
}
