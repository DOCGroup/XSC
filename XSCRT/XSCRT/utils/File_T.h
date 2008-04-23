// -*- C++ -*-

//=============================================================================
/**
 * @file        File_T.h
 *
 * $Id$
 *
 * @author      James H. Hill
 */
//=============================================================================

#include "File.h"
#include <string>
#include <memory>

namespace XSCRT
{
namespace utils
{
/**
 * @struct reader_function
 *
 * Trait for determining an XSC element's reader function signature.
 */
template <typename T>
struct reader_function
{
  /// Type definition of the reading function signature.
  typedef T (* result_type) (const xercesc::DOMDocument *);
};

/**
 * @struct writer_function
 *
 * Trait for determining an XSC element's writer function signature.
 */
template <typename T>
struct writer_function
{
  /// Type definition of the writing function signature.
  typedef void (* result_type) (T const &, xercesc::DOMDocument *);
};

/**
 * @class File_Reader_T
 *
 * Helper class for reading XML documents using XSC.
 */
template <typename T, typename CHAR_TYPE = char>
class File_Reader_T : public File
{
public:
  /// Type definition of the entity type.
  typedef T entity_type;

  /// Type definition of the character type.
  typedef CHAR_TYPE char_type;

  /// Default constructor.
  File_Reader_T (typename reader_function <T>::result_type reader);

  /// Destructor.
  virtual ~File_Reader_T (void);

  /**
   * Open the file for reading. This will open the \a filename if
   * it exists, and populate contained Xerces-C document.
   *
   * @param[in]         filename        Name of the XML file.
   * @retval            0               Successfully opened file.
   * @retval            -1              Failed to open file.
   */
  int open (const CHAR_TYPE * filename);

  /**
   * Get the underlying Xerces-C parser. This enables the client 
   * to configure the parser as needed, e.g., setting/unsetting 
   * features.
   *
   * @return            Pointer to the parser.
   */
  xercesc::DOMBuilder * parser (void);

  /**
   * Extraction operator.
   *
   * @param[in]         entity        Target entity.
   * @return            Reference to the file reader.
   */
  File_Reader_T & operator >> (T & entity);

private:
  /// The entity read from the document.
  typename reader_function <T>::result_type reader_;

	/// xerces-c builder responsible for reading XML.
	xercesc::DOMBuilder * parser_;
};

/**
 * @class File_Reader_T
 *
 * Helper class for reading XML documents using XSC.
 */
template <typename T, typename CHAR_TYPE = char>
class File_Writer_T : public File 
{
public:
  /// Type definition of the entity type.
  typedef T entity_type;

  /// Type definition of the character type.
  typedef CHAR_TYPE char_type;

  /**
   * Initializing constructor.
   *
   * @param[in]       ns          Target namespace.
   * @param[in]       root        Name of the root element.
   * @param[in]       writer      The XSC writer function.
   */
  File_Writer_T (const CHAR_TYPE * ns,
                 const CHAR_TYPE * root,
                 typename writer_function <T>::result_type writer);

  /// Destructor.
  virtual ~File_Writer_T (void);

  /**
   * Open the file for reading. This will open the \a filename if
   * it exists, and populate contained Xerces-C document.
   *
   * @param[in]         filename        Name of the XML file.
   * @retval            0               Successfully opened file.
   * @retval            -1              Failed to open file.
   */
  int open (const CHAR_TYPE * filename);

  /**
   * Extraction operator.
   *
   * @param[in]         entity        Target entity.
   * @return            Reference to the file reader.
   */
  File_Writer_T & operator << (T & entity);

  /**
   * Get the contained DOM writer.
   *
   * @return            Pointer to the DOM writer.
   */
  xercesc::DOMWriter * writer (void);

private:
  /// The entity read from the document.
  typename writer_function <T>::result_type writer_;

	/// xerces-c builder responsible for reading XML.
	xercesc::DOMWriter * dom_writer_;

	/// Target output for the writer.
  std::auto_ptr <xercesc::XMLFormatTarget> target_;
};
}
}

#include "File_T.cpp"

