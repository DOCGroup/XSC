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
#include "xercesc/framework/MemBufFormatTarget.hpp"

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

  bool write (char * buffer, size_t & size);

  /**
   * Insertion operator.
   *
   * @param[in]         entity        Target entity.
   * @return            Reference to the file reader.
   */
  void operator <<= (const T & entity);

  /**
   * Get the size of the buffer.
   *
   * @return            Size of the buffer in bytes.
   */
  size_t get_buffer_size (void) const;

private:
  std::auto_ptr <xercesc::MemBufFormatTarget> target_;
};

}
}

#include "Buffer_Writer_T.cpp"

#endif  // !defined _XSCRT_BUFFER_WRITER_H_
