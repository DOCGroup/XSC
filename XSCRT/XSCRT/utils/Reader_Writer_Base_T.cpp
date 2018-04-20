#include "xercesc/util/XMLUniDefs.hpp"

namespace XSCRT
{
namespace utils
{
//
// Reader_Writer_Base_T
//
template <typename T, typename CHAR_TYPE>
Reader_Writer_Base_T <T, CHAR_TYPE>::Reader_Writer_Base_T (void)
: impl_ (0),
  document_ (0)
{
  xercesc::XMLPlatformUtils::Initialize ();

  // Get default implementation, which is Load-Store (LS) interface.
  static const XMLCh gLS[] = { xercesc::chLatin_L,
                               xercesc::chLatin_S,
                               xercesc::chNull };

  this->impl_ =
    xercesc::DOMImplementationRegistry::getDOMImplementation (gLS);
}

//
// ~Reader_Writer_Base_T
//
template <typename T, typename CHAR_TYPE>
Reader_Writer_Base_T <T, CHAR_TYPE>::~Reader_Writer_Base_T (void)
{
  if (this->document_ != 0)
    this->document_->release ();
}

template <typename T, typename CHAR_TYPE>
xercesc::DOMDocument * const
Reader_Writer_Base_T <T, CHAR_TYPE>::document (void)
{
  return this->document_;
}

}
}
