// $Id$

#include "XSC/XercesString.hpp"

namespace XSCRT
{
namespace utils
{
///////////////////////////////////////////////////////////////////////////////
// File_Reader_T
//

//
// File_Reader_T
//
template <typename T, typename CHAR_TYPE>
File_Reader_T <T, CHAR_TYPE>::
File_Reader_T (typename reader_function <T>::result_type reader)
: reader_ (reader),
  parser_ (0)
{

}

//
// File_Reader_T
//
template <typename T, typename CHAR_TYPE>
File_Reader_T <T, CHAR_TYPE>::~File_Reader_T (void)
{

}

//
// open
//
template <typename T, typename CHAR_TYPE>
int File_Reader_T <T, CHAR_TYPE>::
open (const CHAR_TYPE * filename)
{
  // Close the current document.
  this->close ();

  // Parse the new file to create a new document.
  this->document_ = this->parser ()->parseURI (filename);

  // Return the open status.
  this->is_open_ = this->document_ != 0 ? true : false;
  return this->is_open_ ? 0 : -1;
}

//
// parser
//
template <typename T, typename CHAR_TYPE>
xercesc::DOMBuilder * File_Reader_T <T, CHAR_TYPE>::parser (void)
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
template <typename T, typename CHAR_TYPE>
File_Reader_T <T, CHAR_TYPE> & 
File_Reader_T <T, CHAR_TYPE>::operator >> (T & entity)
{
  if (this->reader_ != 0)
    entity = (*this->reader_) (this->document ());

  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// File_Writer_T
//

//
// File_Writer_T
//
template <typename T, typename CHAR_TYPE>
File_Writer_T <T, CHAR_TYPE>::
File_Writer_T (const CHAR_TYPE * ns,
               const CHAR_TYPE * root,
               typename writer_function <T>::result_type writer)
: writer_ (writer),
  dom_writer_ (0),
  target_ (0)
{
  // Create the default document.
  this->document_ = 
    this->impl ()->createDocument (XSC::XStr (ns), XSC::XStr (root), 0);
}

//
// ~File_Writer_T 
//
template <typename T, typename CHAR_TYPE>
File_Writer_T <T, CHAR_TYPE>::~File_Writer_T (void)
{
  if (this->dom_writer_ != 0) 
  {
    // Release the DOM writer's resources.
    this->dom_writer_->release();
    this->dom_writer_ = 0;
	}
}

//
// open
//
template <typename T, typename CHAR_TYPE>
int File_Writer_T <T, CHAR_TYPE>::
open (const CHAR_TYPE * filename)
{
  this->target_.reset (new xercesc::LocalFileFormatTarget (filename));
  this->is_open_ = true;
  return 0;
}

//
// operator <<
//
template <typename T, typename CHAR_TYPE>
File_Writer_T <T, CHAR_TYPE> & 
File_Writer_T <T, CHAR_TYPE>::operator << (T & entity)
{
  xercesc::DOMWriter const * dom_writer = this->writer ();

  if (this->document_ != 0 && dom_writer)
  {
    // Serialize the entity into the document.
    (*dom_writer) (entity, this->document_);

    // Write the document to the XML file.
    dom_writer->writeNode (this->target_.get (), *this->document_);
  }

  return *this;
}

//
// writer
//
template <typename T, typename CHAR_TYPE>
xercesc::DOMWriter const * File_Writer_T <T, CHAR_TYPE>::writer (void)
{
  if (this->dom_writer_ == 0)
    this->dom_writer_ = this->impl ()->createDOMWriter ();

  return this->dom_writer_;
}

}
}
