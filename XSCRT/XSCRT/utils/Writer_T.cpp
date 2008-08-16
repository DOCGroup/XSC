// $Id$

#include "XSC/XercesString.hpp"

namespace XSCRT
{
namespace utils
{

//
// Writer_T
//
template <typename T, typename CHAR_TYPE>
Writer_T <T, CHAR_TYPE>::
Writer_T (const CHAR_TYPE * ns,
          const CHAR_TYPE * root,
          typename writer_function <T>::result_type writer)
: writer_ (writer),
  dom_writer_ (0)
{
  // Create an empty XML document.
  this->document_ =
    this->impl_->createDocument (XSC::XStr (ns), XSC::XStr (root), 0);

  // Create a DOMWriter for this writer.
  this->dom_writer_ = this->impl_->createDOMWriter ();
}

//
// ~Writer_T
//
template <typename T, typename CHAR_TYPE>
Writer_T <T, CHAR_TYPE>::~Writer_T (void)
{
  if (this->dom_writer_ != 0)
    this->dom_writer_->release ();
}

//
// operator <<=
//
template <typename T, typename CHAR_TYPE>
void Writer_T <T, CHAR_TYPE>::operator <<= (const T & entity)
{
  if (this->writer_ != 0 && this->document_ != 0)
    (*this->writer_) (entity, this->document_);
}

//
// writer
//
template <typename T,  typename CHAR_TYPE>
xercesc::DOMWriter * const Writer_T <T, CHAR_TYPE>::writer (void)
{
  return this->dom_writer_;
}

}
}
