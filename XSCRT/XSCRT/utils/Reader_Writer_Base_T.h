// -*- C++ -*-

//=============================================================================
/**
 * @file          Reader_Writer_Base_T.h
 *
 * 
 *
 * @author        James H. Hill
 */
//=============================================================================

#ifndef _READER_WRITER_BASE_T_H_
#define _READER_WRITER_BASE_T_H_

#include "xercesc/dom/DOM.hpp"

namespace XSCRT
{
namespace utils
{
/**
 * @class Reader_Writer_Base_T
 */
template <typename T, typename CHAR_TYPE = char>
class Reader_Writer_Base_T
{
public:
  /// Type definition of the entity type.
  typedef T entity_type;

  /// Type definition of the char type.
  typedef CHAR_TYPE char_type;

  /// Destructor.
  ~Reader_Writer_Base_T (void);

  xercesc::DOMDocument * const document (void);

protected:
  /// Default constructor.
  Reader_Writer_Base_T (void);

  /// Implementation shared between all files.
  xercesc::DOMImplementation * impl_;

  /// The underlying Xerces-C document.
  xercesc::DOMDocument * document_;
};

}
}

#include "Reader_Writer_Base_T.cpp"

#endif  // !defined _READER_WRITER_BASE_T_H_
