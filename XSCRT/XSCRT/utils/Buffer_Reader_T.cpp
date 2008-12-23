// $Id$

#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
#include "XSC/utils/XercesString.h"
#include <memory>

namespace XSCRT
{
namespace utils
{
//
// Buffer_Reader_T
//
template <typename T, typename CHAR_TYPE>
Buffer_Reader_T <T, CHAR_TYPE>::
Buffer_Reader_T (typename reader_function <T>::result_type reader,
                 const CHAR_TYPE * fake_id)
: Reader_T <T, CHAR_TYPE> (reader),
  fake_id_ (fake_id)
{

}

//
// ~Buffer_Reader_T
//
template <typename T, typename CHAR_TYPE>
Buffer_Reader_T <T, CHAR_TYPE>::~Buffer_Reader_T (void)
{

}

//
// open
//
template <typename T, typename CHAR_TYPE>
bool Buffer_Reader_T <T, CHAR_TYPE>::read (const char * buffer, size_t size)
{
  using namespace xercesc;

  // Open the file for reading.
  MemBufInputSource input (reinterpret_cast <const XMLByte * const> (buffer),
                           size,
                           this->fake_id_);

  this->parser_->parse (input);

  // Get the document.
  this->document_ = this->parser_->adoptDocument ();
  return this->document_ != 0;
}

}
}
