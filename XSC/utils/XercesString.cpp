#include "XercesString.h"
#include <ostream>

using xercesc::XMLString;

namespace XSC
{

//
// XStr
//
XStr::XStr (void)
  : wstr_ (0),
    cstr_ (0)
{

}

//
// XStr
//
XStr::XStr (const char* str)
  : wstr_ (0),
    cstr_ (0)
{
  this->wstr_ = XMLString::transcode (str);
}

//
// XStr
//
XStr::XStr (XMLCh *wstr)
  : wstr_ (0),
    cstr_ (0)
{
  this->wstr_ = XMLString::replicate (wstr);
}

//
// XStr
//
XStr::XStr (const XMLCh* wstr)
  : wstr_ (0),
    cstr_ (0)
{
  this->wstr_ = XMLString::replicate (wstr);
}

//
// XStr
//
XStr::XStr (const XStr &right)
  : wstr_ (0),
    cstr_ (0)
{
  this->wstr_ = XMLString::replicate (right.wstr_);
}

//
// ~XStr
//
XStr::~XStr (void)
{
  if (0 != this->wstr_)
    XMLString::release (&this->wstr_);

  if (0 != this->cstr_)
    XMLString::release (&this->cstr_);
}

//
// operator =
//
XStr& XStr::operator = (const XStr& rhs)
{
  if (&rhs == this)
    return *this;

  XStr temp (rhs);
  std::swap (this->wstr_, temp.wstr_);
  std::swap (this->cstr_, temp.cstr_);

  return *this;
}

//
// operator =
//
XStr& XStr::operator = (const char * rhs)
{
  XStr temp (rhs);
  std::swap (this->wstr_, temp.wstr_);
  std::swap (this->cstr_, temp.cstr_);

  return *this;
}

//
// begin
//
const XMLCh* XStr::begin (void) const
{
  return this->wstr_;
}

//
// end
//
const XMLCh* XStr::end (void) const
{
  return this->wstr_ + this->size ();
}

//
// append
//
bool XStr::append (const XMLCh *tail)
{
  using namespace xercesc;

  int iTailLen = XMLString::stringLen (tail);
  int iWorkLen = XMLString::stringLen (this->wstr_);
  size_t length = iWorkLen + iTailLen + 1;

  XMLCh * result =
    (XMLCh *) XMLPlatformUtils::fgMemoryManager->allocate (length * sizeof (XMLCh));

  bool bOK = result != 0;

  if (bOK)
  {
    XMLCh *target = result;

    XMLString::moveChars (target, this->wstr_, iWorkLen);
    target += iWorkLen;

    XMLString::moveChars (target, tail, iTailLen);
    target += iTailLen;

    *target++ = 0;

    XMLString::release (&this->wstr_);
    this->wstr_ = result;

    if (0 != this->cstr_)
    {
      XMLString::release (&this->cstr_);
      this->cstr_ = 0;
    }
  }

  return bOK;
}

//
// erase
//
bool XStr::erase (const XMLCh *head, const XMLCh *tail)
{
  bool bOK = head <= tail && head >= begin() && tail <= end();
  if (bOK)
    {
      XMLCh *result = new XMLCh[ size() - (tail - head) + 1 ];
      XMLCh *target = result;
      bOK = target != NULL;

      if (bOK)
        {
          const XMLCh *cursor = begin();

          while (cursor != head) *target++ = *cursor++;
          cursor = tail;
          while ( cursor != end() ) *target++ = *cursor++;
          *target ++ = 0;

          XMLString::release (&this->wstr_);
          this->wstr_ = result;

          if (0 != this->cstr_)
          {
            XMLString::release (&this->cstr_);
            this->cstr_ = 0;
          }
        }
    }
  return bOK;
}

//
// size
//
int XStr::size (void) const
{
  return XMLString::stringLen (this->wstr_);
}

//
// operator []
//
XMLCh XStr::operator [] (const int i)
{
  return this->wstr_[i];
}

//
// operator []
//
XMLCh XStr::operator [] (const int i) const
{
  return this->wstr_[i];
}

//
// release
//
XMLCh * XStr::release (void)
{
  XMLCh * temp = this->wstr_;
  this->wstr_ = 0;

  // Release the C string.
  if (0 != this->cstr_)
  {
    XMLString::release (&this->cstr_);
    this->cstr_ = 0;
  }

  return temp;
}

//
// c_str
//
const char * XStr::c_str (void)
{
  if (0 != this->cstr_)
    return this->cstr_;

  this->cstr_ = XMLString::transcode (this->wstr_);
  return this->cstr_;
}

}


//
// operator ==
//
bool operator == (const XSC::XStr & lhs, const XSC::XStr & rhs)
{
  return 0 == XMLString::compareString (lhs, rhs);
}

//
// operator ==
//
bool operator == (const XSC::XStr & lhs, const XMLCh * rhs)
{
  return 0 == XMLString::compareString (lhs, rhs);
}

//
// operator ==
//
bool operator == (const XMLCh * lhs, const XSC::XStr & rhs)
{
  return 0 == XMLString::compareString (lhs, rhs);
}

//
// operator !=
//
bool operator != (const XSC::XStr & lhs, const XSC::XStr & rhs)
{
  return !(lhs == rhs);
}

//
// operator !=
//
bool operator != (const XMLCh * lhs, const XSC::XStr & rhs)
{
  return !(lhs == rhs);
}

//
// operator !=
//
bool operator != (const XSC::XStr & lhs, const XMLCh * rhs)
{
  return !(lhs == rhs);
}
