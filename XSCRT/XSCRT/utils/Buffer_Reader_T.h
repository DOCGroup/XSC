// -*- C++ -*-

//=============================================================================
/**
 * @file            Buffer_Reader_T.h
 *
 * $Id$
 *
 * @author          James H. Hill
 */
//=============================================================================

#ifndef _XSCRT_BUFFER_READER_T_H_
#define _XSCRT_BUFFER_READER_T_H_

#include "Reader_T.h"
#include "XSC/XercesString.hpp"

namespace XSCRT
{
namespace utils
{
/**
 * @class Buffer_Reader_T
 *
 * Utility class for reading a XML document from a file.
 */
template <typename T, typename CHAR_TYPE = char>
class Buffer_Reader_T : public Reader_T <T, CHAR_TYPE>
{
public:
  /// Type definition of the character type.
  typedef typename Reader_T <T, CHAR_TYPE>::char_type char_type;

  /// Type definition of the entity type.
  typedef typename Reader_T <T, CHAR_TYPE>::entity_type entity_type;

  /**
   * Initializing constructor.
   *
   * @param[in]       reader        Target reader function
   */
  Buffer_Reader_T (typename reader_function <T>::result_type reader,
                   const CHAR_TYPE * fake_id);

  /// Destructor.
  ~Buffer_Reader_T (void);

  /**
   * Open the file for reading. This will open the \a filename if
   * it exists, and populate contained Xerces-C document.
   *
   * @param[in]         buffer          Buffer with XML document.
   * @param[in]         size            Size of XML document.
   * @retval            true            Successfully read XML document.
   * @retval            false           Failed to read XML document.
   */
  bool read (const char * buffer, size_t size);

private:
  /// Fake system id for the reader.
  XSC::XStr fake_id_;
};

}
}

#include "Buffer_Reader_T.cpp"

#endif  // !defined _XSCRT_BUFFER_READER_T_H_
