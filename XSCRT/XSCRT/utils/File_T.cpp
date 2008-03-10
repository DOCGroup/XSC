// $Id$

//#include "xercesc/util/XMLString.hpp"

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

///////////////////////////////////////////////////////////////////////////////
// File_Writer_T
//

//
// File_Writer_T
//
template <typename T, typename C>
File_Writer_T <T, C>::
File_Writer_T (const std::basic_string <C> & ns,
               const std::basic_string <C> & root,
               typename writer_function <T>::result_type writer)
: writer_ (writer),
  dom_writer_ (0),
  target_ (0)
{
  // Convert the strings to a XML string.
  XMLCh * xml_ns = xercesc::XMLString::transcode (ns.c_str ());
  XMLCh * xml_root = xercesc::XMLString::transcode (root.c_str ());

  // Create the default document.
  if (xml_ns != 0 && xml_root != 0)
    this->document_ = this->impl ()->createDocument (xml_ns, xml_root, 0);

  // Release the string resources.
  if (xml_ns != 0)
    xercesc::XMLString::release (&xml_ns);

  if (xml_root != 0)
    xercesc::XMLString::release (&xml_root);
}

//
// ~File_Writer_T 
//
template <typename T, typename C>
File_Writer_T <T, C>::~File_Writer_T (void)
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
template <typename T, typename C>
int File_Writer_T <T, C>::open (const std::basic_string <C> & filename)
{
  // Create a new XML target file for the writer.
  this->target_.reset (
    new xercesc::LocalFileFormatTarget (filename.c_str ()));
}

//
// operator <<
//
template <typename T, typename C>
File_Writer_T <T, C> & File_Writer_T <T, C>::operator << (T & entity)
{
  if (this->document_ != 0 && this->dom_writer_ != 0)
  {
    // Serialize the entity into the document.
    (*this->writer_) (entity, this->document_);

    // Write the document to the XML file.
    this->dom_writer_->writeNode (this->target_.get (), *this->document_);
  }

  return *this;
}

//
// writer
//
template <typename T, typename C>
xercesc::DOMWriter const * File_Writer_T <T, C>::writer (void)
{
  if (this->dom_writer_ == 0)
    this->dom_writer_ = this->impl ()->createDOMWriter ();

  return this->dom_writer_;
}

}
}
