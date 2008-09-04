// $Id$

#include "xercesc/framework/MemBufFormatTarget.hpp"
#include <string.h>

namespace XSCRT
{
namespace utils
{
//
// Buffer_Writer_T
//
template <typename T, typename CHAR_TYPE>
Buffer_Writer_T <T, CHAR_TYPE>::
Buffer_Writer_T (const CHAR_TYPE * ns,
               const CHAR_TYPE * root,
               typename writer_function <T>::result_type writer)
: Writer_T <T, CHAR_TYPE> (ns, root, writer)
{

}

//
// Buffer_Writer_T
//
template <typename T, typename CHAR_TYPE>
Buffer_Writer_T <T, CHAR_TYPE>::~Buffer_Writer_T (void)
{

}

//
// open
//
template <typename T, typename CHAR_TYPE>
bool Buffer_Writer_T <T, CHAR_TYPE>::write (const char * buffer, size_t size)
{
  std::auto_ptr <xercesc::MemBufFormatTarget>
    format_target (new xercesc::MemBufFormatTarget ());

  bool retval = false;

  // Write the element to the target.
  if (this->dom_writer_)
    retval = this->dom_writer_->writeNode (format_target.get (), *this->document_);

  if (retval)
  {
    // Get the size of the XML document.
    size_t copy_n = format_target->getLen ();

    if (size < copy_n)
      copy_n = size;

    ::memcpy (buffer, format_target->getRawBuffer (), copy_n);
  }

  return retval;
}

}
}
