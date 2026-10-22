#include "xercesc/util/XMLUniDefs.hpp"

namespace XSCRT
{
namespace utils
{
//
// Reader_Writer_Base_T
//
template <typename T, typename CHAR_TYPE>
Reader_Writer_Base_T <T, CHAR_TYPE>::Reader_Writer_Base_T ()
: impl_ (nullptr),
  document_ (nullptr)
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
Reader_Writer_Base_T <T, CHAR_TYPE>::~Reader_Writer_Base_T ()
{
  if (this->document_ != nullptr)
    this->document_->release ();
}

template <typename T, typename CHAR_TYPE>
xercesc::DOMDocument * const
Reader_Writer_Base_T <T, CHAR_TYPE>::document ()
{
  return this->document_;
}

}
}
