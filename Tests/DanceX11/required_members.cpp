#include "iad.readonly.hpp"

#include <ace/XML_Utils/XML_Helper.h>
#include <ace/XML_Utils/XercesString.h>
#include <ace/XML_Utils/XSCRT/XML.hpp>

#include <stdexcept>

namespace
{
  bool
  rejected (bool include_name, bool include_referenced_artifact)
  {
    XML::XML_Helper<> helper;
    XERCES_CPP_NAMESPACE::DOMDocument* const document = helper.create_dom (
      ACE_TEXT ("NamedImplementationArtifact"),
      ACE_TEXT ("http://www.omg.org/Deployment"));
    XERCES_CPP_NAMESPACE::DOMElement* const root (document->getDocumentElement ());

    if (include_name)
    {
      XERCES_CPP_NAMESPACE::DOMElement* const name = document->createElementNS (
        static_cast<XMLCh const*> (XML::XStr ("http://www.omg.org/Deployment")),
        static_cast<XMLCh const*> (XML::XStr ("name")));
      name->appendChild (document->createTextNode (
        static_cast<XMLCh const*> (XML::XStr ("artifact"))));
      root->appendChild (name);
    }

    if (include_referenced_artifact)
    {
      root->appendChild (document->createElementNS (
        static_cast<XMLCh const*> (XML::XStr ("http://www.omg.org/Deployment")),
        static_cast<XMLCh const*> (XML::XStr ("referencedArtifact"))));
    }

    bool missing_member_rejected (false);
    try
    {
      XSCRT::XML::Element<char> const element (root);
      Deployment::NamedImplementationArtifact value (element);
    }
    catch (std::invalid_argument const&)
    {
      missing_member_rejected = true;
    }

    document->release ();
    return missing_member_rejected;
  }
}

int
main ()
{
  return rejected (false, true) && rejected (true, false) ? 0 : 1;
}
