// -*- C++ -*-

//=============================================================================
/**
 * @file            File_Reader_T.h
 *
 * $Id$
 *
 * @author          James H. Hill
 */
//=============================================================================

#ifndef _XSCRT_FILE_READER_T_H_
#define _XSCRT_FILE_READER_T_H_

#include "Reader_T.h"

namespace XSCRT
{
namespace utils
{
/**
 * @class File_Reader_T
 *
 * Utility class for reading a XML document from a file.
 */
template <typename T, typename CHAR_TYPE = char>
class File_Reader_T : public Reader_T <T, CHAR_TYPE>
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
  File_Reader_T (typename reader_function <T>::result_type reader);

  /// Destructor.
  ~File_Reader_T (void);

  /**
   * Open the file for reading. This will open the \a filename if
   * it exists, and populate contained Xerces-C document.
   *
   * @param[in]         filename        Name of the XML file.
   * @retval            true            Successfully opened file.
   * @retval            false           Failed to open file.
   */
  bool read (const CHAR_TYPE * filename);
};

}
}

#include "File_Reader_T.cpp"

#endif  // !defined _XSCRT_FILE_READER_T_H_
