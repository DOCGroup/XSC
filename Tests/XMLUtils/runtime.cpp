#include <ace/XML_Utils/XSCRT/XML.hpp>
#include <ace/XML_Utils/XSCRT/Parser.hpp>
#include <ace/XML_Utils/XSCRT/Elements.hpp>
#include <ace/XML_Utils/XSCRT/Traversal.hpp>
#include <ace/XML_Utils/XMLSchema/Types.hpp>
#include <ace/XML_Utils/XMLSchema/TypeInfo.hpp>
#include <ace/XML_Utils/XML_Helper.h>
#include <ace/XML_Utils/XercesString.h>
#include "generated_types.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <locale>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

static_assert (std::is_copy_constructible_v<XSCRT::XML::string>);
static_assert (std::is_copy_assignable_v<XSCRT::XML::string>);

namespace
{
  struct CommaDecimalPoint : std::numpunct<char>
  {
    char do_decimal_point () const override { return ','; }
  };

  struct TypeTraverser : XSCRT::Traversal::Traverser<XSCRT::Type, XSCRT::Type>
  {
    void traverse (XSCRT::Type&) override { ++visits; }
    void traverse (XSCRT::Type const&) override { ++visits; }
    int visits {};
  };

  struct StringTraverser : XSCRT::Traversal::Traverser<XMLSchema::string<char>, XSCRT::Type>
  {
    void traverse (XMLSchema::string<char>&) override { ++visits; }
    void traverse (XMLSchema::string<char> const&) override { ++visits; }
    int visits {};
  };

  struct FailingResolver
  {
    FailingResolver () { throw std::runtime_error ("resolver construction failed"); }
  };

  struct TestIdentity : XSCRT::IdentityProvider
  {
    explicit TestIdentity (int value) : value (value) {}

    bool before (XSCRT::IdentityProvider const& other) const override
    {
      return value < static_cast<TestIdentity const&> (other).value;
    }

    int value;
  };

  struct TestType : XSCRT::Type
  {
  };
}

int main ()
{
  {
    XML::XML_Helper<> helper;
    {
      XML::XStr empty_xstr;
      assert (empty_xstr.size () == 0);
      assert (empty_xstr.end () == nullptr);
      assert (!empty_xstr.append (nullptr));
      XML::XStr xstr_suffix (ACE_TEXT_ALWAYS_CHAR ("x"));
      assert (empty_xstr.append (xstr_suffix.begin ()));
      assert (empty_xstr.size () == 1);
    }

    {
      XSCRT::XML::string value ("copyable XML string");
      XSCRT::XML::string copy (value);
      XSCRT::XML::string assigned (L"temporary");
      assigned = copy;
      XSCRT::XML::string moved (std::move (assigned));
      assert (XSCRT::XML::transcode<char> (moved.c_str (), static_cast<char*> (nullptr)) ==
              "copyable XML string");

      XMLCh const surrogate_pair[] = {
        static_cast<XMLCh> (0xD83D), static_cast<XMLCh> (0xDE00), static_cast<XMLCh> (0)
      };
      std::wstring const decoded (XSCRT::XML::transcode<wchar_t> (surrogate_pair, nullptr));
      if (sizeof (wchar_t) == 2)
      {
        assert (decoded.size () == 2);
        assert (decoded[0] == static_cast<wchar_t> (0xD83D));
        assert (decoded[1] == static_cast<wchar_t> (0xDE00));
      }
      else
      {
        assert (decoded.size () == 1);
        assert (decoded[0] == static_cast<wchar_t> (0x1F600));
      }

      std::unique_ptr<XMLCh[]> encoded (XSCRT::XML::transcode (decoded));
      assert (xercesc::XMLString::equals (encoded.get (), surrogate_pair));
    }

    XERCES_CPP_NAMESPACE::DOMDocument* const document =
      helper.create_dom (ACE_TEXT ("root"), ACE_TEXT ("urn:test"));
    assert (document != nullptr);
    XERCES_CPP_NAMESPACE::DOMElement* const root (document->getDocumentElement ());
    root->appendChild (document->createTextNode (
      static_cast<XMLCh const*> (XML::XStr ("text"))));
    root->appendChild (document->createComment (
      static_cast<XMLCh const*> (XML::XStr ("comment"))));
    root->appendChild (document->createElementNS (
      static_cast<XMLCh const*> (XML::XStr ("urn:test")),
      static_cast<XMLCh const*> (XML::XStr ("child"))));

    XSCRT::XML::Element<char> root_element (root);
    bool root_parent_rejected (false);
    try
    {
      root_element.parent ();
    }
    catch (std::out_of_range const&)
    {
      root_parent_rejected = true;
    }
    assert (root_parent_rejected);

    XERCES_CPP_NAMESPACE::DOMElement const* const const_root (root);
    XSCRT::XML::Element<char> read_only_element (const_root);
    bool element_mutation_rejected (false);
    try
    {
      read_only_element.dom_element ();
    }
    catch (std::logic_error const&)
    {
      element_mutation_rejected = true;
    }
    assert (element_mutation_rejected);

    root->setAttribute (static_cast<XMLCh const*> (XML::XStr ("name")),
                        static_cast<XMLCh const*> (XML::XStr ("value")));
    XERCES_CPP_NAMESPACE::DOMAttr const* const const_attribute (
      root->getAttributeNode (static_cast<XMLCh const*> (XML::XStr ("name"))));
    XSCRT::XML::Attribute<char> read_only_attribute (const_attribute);
    bool attribute_mutation_rejected (false);
    try
    {
      read_only_attribute.value ("replacement");
    }
    catch (std::logic_error const&)
    {
      attribute_mutation_rejected = true;
    }
    assert (attribute_mutation_rejected);

    XSCRT::Parser<char> parser (root_element);
    assert (parser.more_elements ());
    XSCRT::XML::Element<char> child (parser.next_element ());
    assert (child.name () == "child");
    assert (child.parent ().name () == "root");
    assert (!parser.more_elements ());

    std::wstring wide_source;
    if (sizeof (wchar_t) == 2)
    {
      wide_source.push_back (static_cast<wchar_t> (0xD83D));
      wide_source.push_back (static_cast<wchar_t> (0xDE42));
    }
    else
      wide_source.push_back (static_cast<wchar_t> (0x1F642));

    std::unique_ptr<XMLCh[]> wide_xml (
      XSCRT::XML::transcode (wide_source));
    assert (XSCRT::XML::transcode<wchar_t> (
              wide_xml.get (), static_cast<wchar_t*> (nullptr)) == wide_source);

    std::wstring invalid_wide_source;
    invalid_wide_source.push_back (static_cast<wchar_t> (0xD800));
    bool invalid_wide_rejected (false);
    try
    {
      std::unique_ptr<XMLCh[]> invalid_wide_xml (
        XSCRT::XML::transcode (invalid_wide_source));
    }
    catch (std::invalid_argument const&)
    {
      invalid_wide_rejected = true;
    }
    assert (invalid_wide_rejected);

    XERCES_CPP_NAMESPACE::DOMDocument* const generated_document =
      helper.create_dom (ACE_TEXT ("document"), ACE_TEXT ("urn:xsc:xml-utils-test"));
    XERCES_CPP_NAMESPACE::DOMElement* const generated_root (
      generated_document->getDocumentElement ());
    XERCES_CPP_NAMESPACE::DOMElement* const label (generated_document->createElementNS (
      static_cast<XMLCh const*> (XML::XStr ("urn:xsc:xml-utils-test")),
      static_cast<XMLCh const*> (XML::XStr ("label"))));
    label->appendChild (generated_document->createTextNode (
      static_cast<XMLCh const*> (XML::XStr ("sample"))));
    generated_root->appendChild (label);
    XERCES_CPP_NAMESPACE::DOMElement* const count (generated_document->createElementNS (
      static_cast<XMLCh const*> (XML::XStr ("urn:xsc:xml-utils-test")),
      static_cast<XMLCh const*> (XML::XStr ("count"))));
    count->appendChild (generated_document->createTextNode (
      static_cast<XMLCh const*> (XML::XStr ("7"))));
    generated_root->appendChild (count);

    XMLUtils::documentType const generated_value (
      XMLUtils::reader::document (generated_document));
    assert (generated_value.label () == "sample");
    assert (generated_value.count () == 7);
    generated_document->release ();

    bool invalid_numeric_rejected (false);
    XERCES_CPP_NAMESPACE::DOMElement* const invalid =
      document->createElement (static_cast<XMLCh const*> (XML::XStr ("invalid")));
    invalid->appendChild (document->createTextNode (
      static_cast<XMLCh const*> (XML::XStr ("12junk"))));
    try
    {
      XSCRT::XML::Element<char> element (invalid);
      XSCRT::FundamentalType<int> value (element);
    }
    catch (std::invalid_argument const&)
    {
      invalid_numeric_rejected = true;
    }
    assert (invalid_numeric_rejected);

    assert (std::isinf (XSCRT::Details::parse_numeric<char, double> ("INF")));
    assert (std::isinf (XSCRT::Details::parse_numeric<char, float> ("-INF")));
    assert (std::signbit (XSCRT::Details::parse_numeric<char, double> ("-INF")));
    assert (std::isnan (XSCRT::Details::parse_numeric<char, double> ("NaN")));

    std::locale const previous_locale {};
    std::locale::global (std::locale (previous_locale, new CommaDecimalPoint));
    assert ((XSCRT::Details::parse_numeric<char, double> ("1.25") == 1.25));
    std::locale::global (previous_locale);

    bool invalid_special_numeric_rejected (false);
    try
    {
      XSCRT::Details::parse_numeric<char, double> ("INFjunk");
    }
    catch (std::invalid_argument const&)
    {
      invalid_special_numeric_rejected = true;
    }
    assert (invalid_special_numeric_rejected);

    XSCRT::XML::Element<char> negative_unsigned (document->createElement (
      static_cast<XMLCh const*> (XML::XStr ("negative-unsigned"))));
    negative_unsigned.dom_element ()->appendChild (
      document->createTextNode (static_cast<XMLCh const*> (XML::XStr ("-1"))));
    bool negative_unsigned_rejected (false);
    try
    {
      XSCRT::FundamentalType<unsigned int> value (negative_unsigned);
    }
    catch (std::invalid_argument const&)
    {
      negative_unsigned_rejected = true;
    }
    assert (negative_unsigned_rejected);

    XSCRT::XML::Element<char> negative_zero (document->createElement (
      static_cast<XMLCh const*> (XML::XStr ("negative-zero"))));
    negative_zero.dom_element ()->appendChild (
      document->createTextNode (static_cast<XMLCh const*> (XML::XStr ("-0"))));
    XSCRT::FundamentalType<unsigned int> zero (negative_zero);
    assert (static_cast<unsigned int> (zero) == 0);

    XERCES_CPP_NAMESPACE::DOMElement* const unsigned_attribute_element =
      document->createElement (static_cast<XMLCh const*> (XML::XStr ("unsigned-attribute")));
    unsigned_attribute_element->setAttribute (
      static_cast<XMLCh const*> (XML::XStr ("value")),
      static_cast<XMLCh const*> (XML::XStr ("-0")));
    XSCRT::XML::Attribute<char> negative_zero_attribute (
      unsigned_attribute_element->getAttributeNode (
        static_cast<XMLCh const*> (XML::XStr ("value"))));
    XSCRT::FundamentalType<unsigned int> attribute_zero (negative_zero_attribute);
    assert (static_cast<unsigned int> (attribute_zero) == 0);

    XSCRT::XML::Element<char> out_of_range (document->createElement (
      static_cast<XMLCh const*> (XML::XStr ("byte"))));
    out_of_range.dom_element ()->appendChild (
      document->createTextNode (static_cast<XMLCh const*> (XML::XStr ("200"))));
    bool byte_range_rejected (false);
    try
    {
      XSCRT::FundamentalType<signed char> value (out_of_range);
    }
    catch (std::out_of_range const&)
    {
      byte_range_rejected = true;
    }
    assert (byte_range_rejected);

    assert (helper.write_DOM (document, ACE_TEXT ("xml_utils_runtime_test.xml")));
    assert (!helper.write_DOM (nullptr, ACE_TEXT ("xml_utils_runtime_test.xml")));
    std::remove ("xml_utils_runtime_test.xml");
    document->release ();

    helper.terminate_parser ();
    bool uninitialized_rejected (false);
    try
    {
      helper.create_dom (ACE_TEXT ("root"), ACE_TEXT ("urn:test"));
    }
    catch (std::logic_error const&)
    {
      uninitialized_rejected = true;
    }
    assert (uninitialized_rejected);
    uninitialized_rejected = false;
    try
    {
      helper.create_dom (ACE_TEXT ("not-a-file.xml"));
    }
    catch (std::logic_error const&)
    {
      uninitialized_rejected = true;
    }
    assert (uninitialized_rejected);
  }

  {
    bool initialization_error_propagated (false);
    try
    {
      XML::XML_Helper<FailingResolver> helper;
    }
    catch (std::runtime_error const&)
    {
      initialization_error_propagated = true;
    }
    assert (initialization_error_propagated);
  }

  {
    XSCRT::ExtendedTypeInfoMap& map (XSCRT::extended_type_info_map ());
    XMLSchema::TypeInfoInitializer<char> initialize (map);
    XMLSchema::token<char> token ("valid-token");
    TypeTraverser traverser;
    traverser.dispatch (token);
    assert (traverser.visits == 1);

    StringTraverser string_traverser;
    string_traverser.dispatch (token);
    assert (string_traverser.visits == 1);
  }

  {
    TestType type;
    TestIdentity identity (1);
    type.register_id (identity, std::addressof (type));
    assert (type.lookup_id (identity) == std::addressof (type));

    bool duplicate_rejected (false);
    try
    {
      type.register_id (identity, std::addressof (type));
    }
    catch (std::logic_error const&)
    {
      duplicate_rejected = true;
    }
    assert (duplicate_rejected);

    type.unregister_id (identity);
    assert (type.lookup_id (identity) == nullptr);

    bool missing_rejected (false);
    try
    {
      type.unregister_id (identity);
    }
    catch (std::logic_error const&)
    {
      missing_rejected = true;
    }
    assert (missing_rejected);
  }

  {
    TestType root;
    XMLSchema::ID<char> first (std::string ("first"));
    XMLSchema::ID<char> second (std::string ("second"));
    first.container (std::addressof (root));
    second.container (std::addressof (root));

    bool duplicate_rejected (false);
    try
    {
      first = second;
    }
    catch (std::logic_error const&)
    {
      duplicate_rejected = true;
    }

    assert (duplicate_rejected);
    assert (first == "first");
    assert (second == "second");

    first = std::string ("third");
    assert (first == "third");
  }

  {
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

  {
    TestType old_root;
    TestType new_root;
    XMLSchema::ID<char> moved ("moved");
    moved.container (std::addressof (old_root));
    moved.container (std::addressof (new_root));

    XMLSchema::ID<char> old_name_reused ("moved");
    old_name_reused.container (std::addressof (old_root));

    XMLSchema::ID<char> new_name_conflict ("moved");
    bool duplicate_rejected (false);
    try
    {
      new_name_conflict.container (std::addressof (new_root));
    }
    catch (std::logic_error const&)
    {
      duplicate_rejected = true;
    }
    assert (duplicate_rejected);
  }

  {
    TestType old_root;
    TestType new_root;
    XMLSchema::ID<char> moving ("move");
    XMLSchema::ID<char> destination_id ("move");
    moving.container (std::addressof (old_root));
    destination_id.container (std::addressof (new_root));

    bool duplicate_rejected (false);
    try
    {
      moving.container (std::addressof (new_root));
    }
    catch (std::logic_error const&)
    {
      duplicate_rejected = true;
    }
    assert (duplicate_rejected);
    assert (moving.container () == std::addressof (old_root));

    XMLSchema::ID<char> old_root_conflict ("move");
    bool old_registration_restored (false);
    try
    {
      old_root_conflict.container (std::addressof (old_root));
    }
    catch (std::logic_error const&)
    {
      old_registration_restored = true;
    }
    assert (old_registration_restored);
  }
}
