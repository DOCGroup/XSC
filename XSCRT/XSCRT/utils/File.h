// -*- C++ -*-

//=============================================================================
/**
 * @file          File.h
 *
 * $Id$
 *
 * @author        James H. Hill
 */
//=============================================================================

#ifndef _XSCRT_FILE_H_
#define _XSCRT_FILE_H_

#include "xercesc/dom/DOM.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/util/XMLUniDefs.hpp"
#include "XSCRT/XSCRT_export.h"

namespace XSCRT
{
namespace utils
{
/**
 * @class File
 *
 * Base implementation for all XSC's file utility classes. This class
 * ensures that Xerces-C and its elements needed to read/write XML files
 * are initialized/terminated as needed.
 */
class XSCRT_Export File 
{
public:
  /// Default constructor.
  File (void);

  /// Destructor.
  virtual ~File (void);

  /// Close the file.
  void close (void);

  /**
   * Get a pointer to the contained Xerces-C document.
   *
   * @return            Pointer to the contained document.
   */
	xercesc::DOMDocument * document (void);

  /**
   * Pointer to the document's implementation.
   *
   * @return            Pointer to the document's implementation.
   */
  xercesc::DOMImplementation * impl (void);

  /**
   * Get the open state of the file.
   *
   * @retval    true    The file is open.
   * @retval    false   The file is not open.
   */
  bool is_open (void) const;

protected:
  /// Pointer to the DOM document.
	xercesc::DOMDocument * document_;

	/// Implementation shared between all files.
	xercesc::DOMImplementation * impl_;

  /// The open state of the file.
  bool is_open_;

private:
  /// Number of open files.
  static size_t count_;
};
}
}

#endif  // !defined _XSCRT_FILE_H_
