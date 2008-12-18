// -*- C++ -*-

//=============================================================================
/**
 * @file            Reader_T.h
 *
 * $Id$
 *
 * @author          James H. Hill
 */
//=============================================================================

#ifndef _XSCRT_READER_T_H_
#define _XSCRT_READER_T_H_

#include "Reader_Writer_Base_T.h"
#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include <memory>

namespace XSCRT
{
namespace utils
{
/**
 * @struct reader_function
 *
 * Trait for determining an XSC element's reader function signature.
 */
template <typename T>
struct reader_function
{
  /// Type definition of the reading function signature.
  typedef T (* result_type) (const xercesc::DOMDocument *);
};

/**
 * @class Reader_T
 *
 * Base class for reading data from an XML document.
 */
template <typename T, typename CHAR_TYPE = char>
class Reader_T : public Reader_Writer_Base_T <T, CHAR_TYPE>
{
public:
  /// Type definition of the entity type.
  typedef typename Reader_Writer_Base_T <T, CHAR_TYPE>::entity_type entity_type;

  /// Type definition of the character type.
  typedef typename Reader_Writer_Base_T <T, CHAR_TYPE>::char_type char_type;

  /// Destructor.
  virtual ~Reader_T (void);

  /**
   * Extraction operator.
   *
   * @param[in]         entity        Target entity.
   */
  void operator >>= (T & entity) const;

  /**
   * Get the underlying parser. This will allow the client to
   * modify its properties.
   *
   * @return            Pointer to the parser.
   */
  xercesc::XercesDOMParser * operator -> (void);

protected:
  /**
   * Default constructor.
   *
   * @param[in]         reader        Reader function.
   */
  Reader_T (typename reader_function <T>::result_type reader);

  /// DOM parser for the reader.
  std::auto_ptr <xercesc::XercesDOMParser> parser_;

  /// The entity read from the document.
  typename reader_function <T>::result_type reader_;
};
}
}

#include "Reader_T.cpp"

#endif  // !defined _XSCRT_READER_T_H_
