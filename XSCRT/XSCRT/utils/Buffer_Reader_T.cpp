// $Id$

#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
#include "XSC/XercesString.hpp"
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
  std::auto_ptr <MemBufInputSource>
    input (new MemBufInputSource (buffer, size, this->fake_id_));

  Wrapper4InputSource wrapper (input.get ());
  input.release ();

  // Parse the specified file.
  this->document_ = this->parser_->parse (wrapper);
  return this->document_ != 0;
}

}
}
