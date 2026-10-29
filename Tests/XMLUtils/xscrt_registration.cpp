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

  void verify_subtree_rollback (std::string const &conflict_name)
  {
    TestType old_root;
    TestType new_root;
    TestType subtree;
    XMLSchema::ID<char> first (std::string ("first"));
    XMLSchema::ID<char> second (std::string ("second"));
    XMLSchema::ID<char> conflict (conflict_name);

    first.container (std::addressof (subtree));
    second.container (std::addressof (subtree));
    subtree.container (std::addressof (old_root));
    conflict.container (std::addressof (new_root));

    bool move_rejected (false);
    try
    {
      subtree.container (std::addressof (new_root));
    }
    catch (std::logic_error const&)
    {
      move_rejected = true;
    }

    assert (move_rejected);
    assert (subtree.container () == std::addressof (old_root));

    std::string const names[] {"first", "second"};
    for (std::string const &name : names)
    {
      XMLSchema::ID<char> old_root_probe (name);
      bool old_registration_restored (false);
      try
      {
        old_root_probe.container (std::addressof (old_root));
      }
      catch (std::logic_error const&)
      {
        old_registration_restored = true;
      }
      assert (old_registration_restored);

      if (name != conflict_name)
      {
        XMLSchema::ID<char> new_root_probe (name);
        new_root_probe.container (std::addressof (new_root));
      }
    }
  }
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

  // Trying both conflict positions covers a partial target registration
  // regardless of the map's pointer ordering.
  verify_subtree_rollback ("first");
  verify_subtree_rollback ("second");
}
