#include <memory>

#include "xercesc/framework/LocalFileFormatTarget.hpp"

namespace XSCRT
{
namespace utils
{
//
// File_Writer_T
//
template <typename T, typename CHAR_TYPE>
File_Writer_T <T, CHAR_TYPE>::
File_Writer_T (const CHAR_TYPE * ns,
               const CHAR_TYPE * root,
               typename writer_function <T>::result_type writer)
: Writer_T <T, CHAR_TYPE> (ns, root, writer)
{

}

//
// File_Writer_T
//
template <typename T, typename CHAR_TYPE>
File_Writer_T <T, CHAR_TYPE>::~File_Writer_T () = default;

//
// open
//
template <typename T, typename CHAR_TYPE>
bool File_Writer_T <T, CHAR_TYPE>::write (const CHAR_TYPE * filename)
{
  auto target_file = std::make_unique<xercesc::LocalFileFormatTarget> (
    XSC::XStr (filename));

  // Write the element to the target.
  if (this->dom_writer_)
    return this->dom_writer_->writeNode (target_file.get (), *this->document_);

  return false;
}

}
}
