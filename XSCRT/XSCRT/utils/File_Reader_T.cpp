// $Id$

#include "XSC/utils/XercesString.h"

namespace XSCRT
{
namespace utils
{
//
// File_Reader_T
//
template <typename T, typename CHAR_TYPE>
File_Reader_T <T, CHAR_TYPE>::
File_Reader_T (typename reader_function <T>::result_type reader)
: Reader_T <T, CHAR_TYPE> (reader)
{

}

//
// ~File_Reader_T
//
template <typename T, typename CHAR_TYPE>
File_Reader_T <T, CHAR_TYPE>::~File_Reader_T (void)
{

}

//
// open
//
template <typename T, typename CHAR_TYPE>
bool File_Reader_T <T, CHAR_TYPE>::read (const CHAR_TYPE * filename)
{
  // Parse the specified file.
  this->parser_->parse (filename);
  this->document_ = this->parser_->adoptDocument ();

  return this->document_ != 0;
}

}
}
