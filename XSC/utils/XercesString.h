// $Id$

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
    XStr() : _wstr(0L) { };

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

    operator const XMLCh* () const { return _wstr; };

    XMLCh * release (void);

  private:

    XMLCh* _wstr; // Internal representation

  };

}

#endif /* _XERCESSTRING_H */
