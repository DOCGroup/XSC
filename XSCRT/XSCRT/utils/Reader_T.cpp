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
: parser_ (new xercesc::XercesDOMParser ()),
  reader_ (reader)
{

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
xercesc::XercesDOMParser * Reader_T <T, CHAR_TYPE>::operator -> (void)
{
  return this->parser_.get ();
}

}
}
