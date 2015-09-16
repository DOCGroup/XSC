#include "dom.hpp"

#include <xercesc/util/XMLUniDefs.hpp>

#include <iostream>

using std::wcerr;
using std::endl;

namespace
{
  std::wostream&
  operator<< (std::wostream& o, XMLCh const* str)
  {
    char* s = xercesc::XMLString::transcode (str);

    o << s;

    xercesc::XMLString::release (&s); // idiot?
    return o;
  }
}

// ErrorHandler
//

ErrorHandler::
ErrorHandler ()
    : failed_ (false)
{
}

bool ErrorHandler::
handleError (xercesc::DOMError const& e)
{
  wcerr << e.getLocation()->getURI() << L':'
        << e.getLocation()->getLineNumber() << L'.'
        << e.getLocation()->getColumnNumber();

  switch (e.getSeverity())
  {
  case xercesc::DOMError::DOM_SEVERITY_WARNING:
    {
      wcerr << L" warning: ";
      break;
    }
  default:
    {
      wcerr << L" error: ";
      failed_ = true;
      break;
    }
  }

  wcerr << e.getMessage() << endl;

  return true;
}

bool ErrorHandler::
failed () const
{
  return failed_;
}


xercesc::DOMDocument*
dom (char const* name)
{
  try
  {
    using namespace xercesc;

    // Initialize Xerces runtime
    //
    XMLPlatformUtils::Initialize();

    // Instantiate the DOM parser.
    //
    XMLCh const gLS[] = {chLatin_L, chLatin_S, chNull };

    // Get an implementation of the Load-Store (LS) interface.
    //
    DOMImplementationLS* impl =
      static_cast<DOMImplementationLS*>(
        DOMImplementationRegistry::getDOMImplementation(gLS));

    // Create a DOMBuilder.
    //
    DOMBuilder* parser =
      impl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    // Discard comment nodes in the document.
    //
    parser->setFeature (XMLUni::fgDOMComments, false);

    // Disable datatype normalization. The XML 1.0 attribute value
    // normalization always occurs though.
    //
    parser->setFeature (XMLUni::fgDOMDatatypeNormalization, true);

    // Do not create EntityReference nodes in the DOM tree. No
    // EntityReference nodes will be created, only the nodes
    // corresponding to their fully expanded substitution text will be
    // created.
    //
    parser->setFeature (XMLUni::fgDOMEntities, false);

    // Perform Namespace processing.
    //
    parser->setFeature (XMLUni::fgDOMNamespaces, true);

    // Perform Validation
    //
    parser->setFeature (XMLUni::fgDOMValidation, true);

    // Do not include ignorable whitespace in the DOM tree.
    //
    parser->setFeature (XMLUni::fgDOMWhitespaceInElementContent, false);

    // Enable the parser's schema support.
    //
    parser->setFeature (XMLUni::fgXercesSchema, true);

    // Enable full schema constraint checking, including checking which
    // may be time-consuming or memory intensive. Currently, particle
    // unique attribution constraint checking and particle derivation
    // restriction checking are controlled by this option.
    //
    parser->setFeature (XMLUni::fgXercesSchemaFullChecking, true);

    // The parser will treat validation error as fatal and will exit.
    //
    parser->setFeature (XMLUni::fgXercesValidationErrorAsFatal, true);

    ::ErrorHandler eh;
    parser->setErrorHandler(&eh);

    DOMDocument* doc = parser->parseURI(name);

    if (!eh.failed ()) return doc;
  }
  catch (xercesc::DOMException const&)
  {
    wcerr << L"caught DOMException" << endl;
  }
  catch (xercesc::XMLException const&)
  {
    wcerr << L"caught XMLException" << endl;
  }
  catch (...)
  {
    wcerr << L"caught unknown exception" << endl;
  }

  return 0;
}
