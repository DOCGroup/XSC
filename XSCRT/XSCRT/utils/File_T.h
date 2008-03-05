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
 * @class File_Reader_T
 *
 * Helper class for reading XML documents using XSC.
 */
template <typename T, typename C = char>
class File_Reader_T : File
{
public:
  /// Type definition of the entity type.
  typedef T entity_type;

  /// Type definition of the character type.
  typedef C char_type;

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
  int open (const std::basic_string <C> & filename);

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

}
}

#include "File_T.cpp"

