// -*- C++ -*-

//=============================================================================
/**
 * @file        Console_Error_Handler.h
 *
 * $Id$
 *
 * @author      James H. Hill
 */
//=============================================================================

#ifndef _XSCRT_CONSOLE_ERROR_HANDLER_H_
#define _XSCRT_CONSOLE_ERROR_HANDLER_H_

#include "xercesc/dom/DOMErrorHandler.hpp"
#include "../XSCRT_export.h"

namespace XSCRT
{
namespace utils
{
/**
 * @class Console_Error_Handler
 *
 * Xerces-C error handler for printing messages to the console.
 */
class XSCRT_Export Console_Error_Handler : public xercesc::DOMErrorHandler
{
public:
  /// Default constructor.
  Console_Error_Handler (void);

  /// Destructor.
  virtual ~Console_Error_Handler (void);

  /**
   * Handle an encountered error.
   */
  virtual bool handleError (const xercesc::DOMError & error);
};
}
}

#endif  // !defined _XSCRT_CONSOLE_ERROR_HANDLER_H_
