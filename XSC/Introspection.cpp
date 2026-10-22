// file      : XSC/Introspection.cpp

#include "Utility/Introspection/Object.hpp"

namespace Utility::Introspection
{
  namespace
  {
    TypeInfo object_ {typeid (Object)};
  }

  TypeInfo const& Object::
  static_type_info () noexcept
  {
    return object_;
  }
}
