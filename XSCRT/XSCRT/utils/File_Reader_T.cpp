// $Id$

#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
#include "XSC/XercesString.hpp"
#include <memory>

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
  using namespace xercesc;

  // Open the file for reading.
  std::auto_ptr <LocalFileInputSource>
    input (new LocalFileInputSource (XSC::XStr (filename)));

  Wrapper4InputSource wrapper (input.get ());
  input.release ();

  // Parse the specified file.
  this->document_ = this->parser_->parse (wrapper);
  return this->document_ != 0;
}

}
}
