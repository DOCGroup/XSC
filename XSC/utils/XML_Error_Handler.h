//==============================================================
/**
 *  @file  XML_Error_Handler.h
 *
 *  @brief Error handler for Xerces
 *
 *  @author Bala Natarajan <bala@dre.vanderbilt.edu>
 */
//================================================================
#ifndef CIAO_XML_ERROR_HANDLER_H
#define CIAO_XML_ERROR_HANDLER_H

#include "XSC_Utils_export.h"
#include "xercesc/sax/ErrorHandler.hpp"

using xercesc::SAXParseException;

namespace XSC
{
  namespace XML
  {
    /**
     * @class XML_Error_Hander
     *
     * @brief Error handler for XERCES
     *
     */
    class XSC_UTILS_Export XML_Error_Handler
      : public xercesc::ErrorHandler
    {
    public:

      XML_Error_Handler (void);

      ~XML_Error_Handler (void);

      void warning(const SAXParseException& toCatch);
      void error(const SAXParseException& toCatch);
      void fatalError(const SAXParseException& toCatch);
      void resetErrors();
      bool getErrors (void) const;
    private :
      // Disallow copying
      XML_Error_Handler (const XML_Error_Handler&);
      XML_Error_Handler& operator= (const XML_Error_Handler&);

      bool errors_;
    };
  }
}


#endif /* CIAO_XML_ERROR_HANDLER_H*/
