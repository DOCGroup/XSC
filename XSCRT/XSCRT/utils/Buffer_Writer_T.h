// -*- C++ -*-

//=============================================================================
/**
 * @file            Buffer_Writer_T.h
 *
 * $Id$
 *
 * @author          James H. Hill
 */
//=============================================================================

#ifndef _XSCRT_BUFFER_WRITER_H_
#define _XSCRT_BUFFER_WRITER_H_

#include "Writer_T.h"

namespace XSCRT
{
namespace utils
{

/**
 * @class Buffer_Writer_T
 */
template <typename T, typename CHAR_TYPE = char>
class Buffer_Writer_T : public Writer_T <T, CHAR_TYPE>
{
public:
  /**
   * Initializing constructor.
   *
   * @param[in]         ns        Target namespace
   * @param[in]         root      Name of root element
   * @param[in]         writer    Writer function
   */
  Buffer_Writer_T (const CHAR_TYPE * ns,
                   const CHAR_TYPE * root,
                   typename writer_function <T>::result_type writer);

  /// Destructor.
  ~Buffer_Writer_T (void);

  bool write (const CHAR_TYPE * filename);
};

}
}

#include "Buffer_Writer_T.cpp"

#endif  // !defined _XSCRT_BUFFER_WRITER_H_
