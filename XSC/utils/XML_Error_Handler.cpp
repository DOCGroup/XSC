// $Id$

#include "XML_Error_Handler.h"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOMLocator.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include "XercesString.h"
#include <memory>
#include <iostream>

using xercesc::SAXParseException;

namespace XSC
{
  namespace XML
  {
    XML_Error_Handler::XML_Error_Handler (void)
      : errors_ (false)
    {
    }

    XML_Error_Handler::~XML_Error_Handler()
    {
    }

    void XML_Error_Handler::warning(const SAXParseException& toCatch)
    {
      char * file = xercesc::XMLString::transcode (toCatch.getSystemId ());
      char * msg = xercesc::XMLString::transcode (toCatch.getMessage ());

      std::cerr << "Warning: " << file << ':' << toCatch.getLineNumber ()
                << ':' << toCatch.getColumnNumber () << " - "
                << msg << std::endl;

      xercesc::XMLString::release (&file);
      xercesc::XMLString::release (&msg);
    }

    void XML_Error_Handler::error(const SAXParseException& toCatch)
    {
      char * file = xercesc::XMLString::transcode (toCatch.getSystemId ());
      char * msg = xercesc::XMLString::transcode (toCatch.getMessage ());

      std::cerr << "Error: " << file << ':' << toCatch.getLineNumber ()
                << ':' << toCatch.getColumnNumber () << " - "
                << msg << std::endl;

      this->errors_ = true;

      xercesc::XMLString::release (&file);
      xercesc::XMLString::release (&msg);
    }

    void XML_Error_Handler::fatalError(const SAXParseException& toCatch)
    {
      char * file = xercesc::XMLString::transcode (toCatch.getSystemId ());
      char * msg = xercesc::XMLString::transcode (toCatch.getMessage ());

      std::cerr << "Fatal Error: " << file << ':' << toCatch.getLineNumber ()
                << ':' << toCatch.getColumnNumber () << " - "
                << msg << std::endl;

      this->errors_ = true;

      xercesc::XMLString::release (&file);
      xercesc::XMLString::release (&msg);
    }

    void XML_Error_Handler::resetErrors()
    {
      this->errors_ = false;
    }

    bool
    XML_Error_Handler::getErrors (void) const
    {
      return this->errors_;
    }
  }
}
