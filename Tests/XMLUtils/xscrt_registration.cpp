#include <ace/XML_Utils/XSCRT/XMLSchema.hpp>

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>

namespace
{
  struct TestType : XSCRT::Type
  {
  };
}

int main ()
{
  XSCRT::ExtendedTypeInfoMap type_info;
  XMLSchema::TypeInfoInitializer<char> initialize (type_info);

  TestType root;
  XMLSchema::ID<char> existing (std::string ("duplicate"));
  XMLSchema::ID<char> rejected (std::string ("duplicate"));
  existing.container (std::addressof (root));

  bool duplicate_rejected (false);
  try
  {
    rejected.container (std::addressof (root));
  }
  catch (std::logic_error const&)
  {
    duplicate_rejected = true;
  }

  assert (duplicate_rejected);
}
