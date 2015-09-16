#ifndef _XERCESSTRING_H
#define _XERCESSTRING_H

#include "XSC_Utils_export.h"
#include "xercesc/util/XMLString.hpp"
#include <iosfwd>

// Utility class that provides a std::string like facade to XMLString.
// Doesn't implement all of the methods of std::string.
namespace XSC
{
  class XSC_UTILS_Export XStr
  {
  public:
    XStr (void);

    XStr (const char* str);

    XStr (XMLCh* wstr);

    XStr (const XMLCh* wstr);

    XStr (const XStr& copy);

    XStr& operator= (const XStr& rhs);

    XStr& operator= (const char * rhs);

    ~XStr();

    const XMLCh* begin() const;

    const XMLCh* end() const;

    bool append(const XMLCh* tail);

    bool erase (const XMLCh* head, const XMLCh* tail);

    int size() const;

    XMLCh operator [] (const int i);

    XMLCh operator [] (const int i) const;

    operator const XMLCh* () const { return wstr_; };

    XMLCh * release (void);

    /// Get the C string version of the XercesString.
    const char * c_str (void);

  private:
    /// Internal representation
    XMLCh* wstr_;

    /// The C string version of the XercesString.
    char * cstr_;
  };
}

XSC_UTILS_Export bool operator == (const XSC::XStr & lhs, const XSC::XStr & rhs);
XSC_UTILS_Export bool operator == (const XMLCh * lhs, const XSC::XStr & rhs);
XSC_UTILS_Export bool operator == (const XSC::XStr & lhs, const XMLCh * rhs);

XSC_UTILS_Export bool operator != (const XSC::XStr & lhs, const XSC::XStr & rhs);
XSC_UTILS_Export bool operator != (const XMLCh * lhs, const XSC::XStr & rhs);
XSC_UTILS_Export bool operator != (const XSC::XStr & lhs, const XMLCh * rhs);

#endif /* _XERCESSTRING_H */
