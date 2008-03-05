// $Id$

///////////////////////////////////////////////////////////////////////////////
// File_Reader
//

namespace XSCRT
{
namespace utils
{
//
// File_Reader_T
//
template <typename T, typename C>
File_Reader_T <T, C>::
File_Reader_T (typename reader_function <T>::result_type reader)
: reader_ (reader),
  parser_ (0)
{

}

//
// File_Reader_T
//
template <typename T, typename C>
File_Reader_T <T, C>::~File_Reader_T (void)
{

}

//
// open
//
template <typename T, typename C>
int File_Reader_T <T, C>::
open (const std::basic_string <C> & filename)
{
  // Close the current document.
  this->close ();

  // Parse the new file to create a new document.
  this->document_ = this->parser ()->parseURI (filename.c_str ());

  // Return the open status.
  return this->document_ != 0 ? 0 : -1;
}

//
// parser
//
template <typename T, typename C>
xercesc::DOMBuilder * File_Reader_T <T, C>::parser (void)
{
  using namespace xercesc;

  if (this->parser_ == 0)
  {
    DOMImplementationLS * impl = 
      dynamic_cast <DOMImplementationLS *> (this->impl ());

    this->parser_ =
      impl->createDOMBuilder (DOMImplementationLS::MODE_SYNCHRONOUS, 0);
  }

  return this->parser_;
}

//
// read
//
template <typename T, typename C>
File_Reader_T <T, C> & File_Reader_T <T, C>::operator >> (T & entity)
{
  if (this->reader_ != 0)
    entity = (*this->reader_) (this->document ());

  return *this;
}

}
}
