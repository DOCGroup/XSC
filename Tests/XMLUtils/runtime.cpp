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
#include <cstdio>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

static_assert (std::is_copy_constructible_v<XSCRT::XML::string>);
static_assert (std::is_copy_assignable_v<XSCRT::XML::string>);

namespace
{
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
}

int main ()
{
  {
    XML::XML_Helper<> helper;
    {
      XSCRT::XML::string value ("copyable XML string");
      XSCRT::XML::string copy (value);
      XSCRT::XML::string assigned (L"temporary");
      assigned = copy;
      XSCRT::XML::string moved (std::move (assigned));
      assert (XSCRT::XML::transcode<char> (moved.c_str (), static_cast<char*> (nullptr)) ==
              "copyable XML string");
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
    XSCRT::Parser<char> parser (root_element);
    assert (parser.more_elements ());
    XSCRT::XML::Element<char> child (parser.next_element ());
    assert (child.name () == "child");
    assert (!parser.more_elements ());

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
}
