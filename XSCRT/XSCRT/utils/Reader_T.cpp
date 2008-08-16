// $Id$

#include "xercesc/util/XMLUniDefs.hpp"

namespace XSCRT
{
namespace utils
{
//
// File_Reader_T
//
template <typename T, typename CHAR_TYPE>
Reader_T <T, CHAR_TYPE>::
Reader_T (typename reader_function <T>::result_type reader)
: parser_ (0),
  reader_ (reader)
{
  // Create an DOMBuilder from the implementation.
  this->parser_ =
    this->impl_->createDOMBuilder (xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
}

//
// ~File_Reader_T
//
template <typename T, typename CHAR_TYPE>
Reader_T <T, CHAR_TYPE>::~Reader_T (void)
{

}

//
// operator >>=
//
template <typename T, typename CHAR_TYPE>
void Reader_T <T, CHAR_TYPE>::operator >>= (T & entity) const
{
  if (this->reader_ && this->document_)
    entity = (*this->reader_) (this->document_);
}

//
// parser
//
template <typename T, typename CHAR_TYPE>
xercesc::DOMBuilder * const Reader_T <T, CHAR_TYPE>::parser (void)
{
  return this->parser_;
}

}
}
