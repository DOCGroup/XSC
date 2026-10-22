#include "XSC/utils/XercesString.h"

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
  dom_writer_ (nullptr)
{
  // Create an empty XML document.
  this->document_ =
    this->impl_->createDocument (XSC::XStr (ns), XSC::XStr (root), nullptr);

  // Create a DOMWriter for this writer.
  this->dom_writer_ =
    ((xercesc::DOMImplementationLS *)this->impl_)->createLSSerializer ();
}

//
// ~Writer_T
//
template <typename T, typename CHAR_TYPE>
Writer_T <T, CHAR_TYPE>::~Writer_T ()
{
  if (this->dom_writer_ != nullptr)
    this->dom_writer_->release ();
}

//
// operator <<=
//
template <typename T, typename CHAR_TYPE>
void Writer_T <T, CHAR_TYPE>::operator <<= (const T & entity)
{
  if (this->writer_ != nullptr && this->document_ != nullptr)
    (*this->writer_) (entity, this->document_);
}

//
// writer
//
template <typename T,  typename CHAR_TYPE>
xercesc::DOMLSSerializer * const Writer_T <T, CHAR_TYPE>::writer ()
{
  return this->dom_writer_;
}

}
}
