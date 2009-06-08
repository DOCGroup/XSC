// $Id$

//=============================================================================
/**
 * @file            Writer_T.h
 *
 * $Id$
 *
 * @author          James H. Hill
 */
//=============================================================================

#ifndef _XSCRT_WRITER_T_H_
#define _XSCRT_WRITER_T_H_

#include <memory>
#include "xercesc/framework/XMLFormatter.hpp"
#include "Reader_Writer_Base_T.h"

namespace XSCRT
{
namespace utils
{
/**
 * @struct writer_function
 *
 * Trait for determining an XSC element's writer function signature.
 */
template <typename T>
struct writer_function
{
  /// Type definition of the writing function signature.
  typedef void (* result_type) (T const &, xercesc::DOMDocument *);
};

/**
 * @class Writer_T
 *
 *
 */
template <typename T, typename CHAR_TYPE = char>
class Writer_T : public Reader_Writer_Base_T <T, CHAR_TYPE>
{
public:
  /// Destructor.
  ~Writer_T (void);

  /**
   * Insertion operator.
   *
   * @param[in]         entity        Target entity.
   * @return            Reference to the file reader.
   */
  void operator <<= (const T & entity);

  xercesc::DOMLSSerializer * const writer (void);

protected:
  /**
   *
   */
  Writer_T (const CHAR_TYPE * ns,
            const CHAR_TYPE * root,
            typename writer_function <T>::result_type writer);

  typename writer_function <T>::result_type writer_;

  xercesc::DOMLSSerializer * dom_writer_;
};

}
}

#include "Writer_T.cpp"

#endif  // !defined _XSCRT_WRITER_T_H_
