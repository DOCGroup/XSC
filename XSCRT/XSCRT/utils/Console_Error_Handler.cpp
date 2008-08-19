// $Id$

#include "Console_Error_Handler.h"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOMLocator.hpp"
#include <iostream>

namespace XSCRT
{
namespace utils
{

//
// Console_Error_Handler
//
Console_Error_Handler::Console_Error_Handler (void)
{

}

//
// ~Console_Error_Handler
//
Console_Error_Handler::~Console_Error_Handler (void)
{

}

//
// handleError
//
bool Console_Error_Handler::handleError (const xercesc::DOMError & error)
{
  char * msg =
    xercesc::XMLString::transcode (error.getMessage ());

  xercesc::DOMLocator * locator = error.getLocation ();

  std::cerr
    << "(line " << locator->getLineNumber ()
    << ", column " << locator->getColumnNumber ()
    << "): " << msg << std::endl;

  delete [] msg;
  return false;
}

}
}
