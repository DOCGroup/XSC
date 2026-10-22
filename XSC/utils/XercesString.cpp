#include "XercesString.h"
#include <cstddef>
#include <ostream>
#include <utility>

using xercesc::XMLString;

namespace XSC
{

//
// XStr
//
XStr::XStr ()
{

}

//
// XStr
//
XStr::XStr (const char* str)
{
  this->wstr_ = XMLString::transcode (str);
}

//
// XStr
//
XStr::XStr (XMLCh *wstr)
{
  this->wstr_ = XMLString::replicate (wstr);
}

//
// XStr
//
XStr::XStr (const XMLCh* wstr)
{
  this->wstr_ = XMLString::replicate (wstr);
}

//
// XStr
//
XStr::XStr (const XStr &right)
{
  this->wstr_ = XMLString::replicate (right.wstr_);
}

//
// XStr
//
XStr::XStr (XStr&& right) noexcept
  : wstr_ (std::exchange (right.wstr_, nullptr)),
    cstr_ (std::exchange (right.cstr_, nullptr))
{
}

//
// ~XStr
//
XStr::~XStr ()
{
  if (this->wstr_ != nullptr)
    XMLString::release (&this->wstr_);

  if (this->cstr_ != nullptr)
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
XStr& XStr::operator = (XStr&& rhs) noexcept
{
  if (&rhs == this)
    return *this;

  if (this->wstr_ != nullptr)
    XMLString::release (&this->wstr_);

  if (this->cstr_ != nullptr)
    XMLString::release (&this->cstr_);

  this->wstr_ = std::exchange (rhs.wstr_, nullptr);
  this->cstr_ = std::exchange (rhs.cstr_, nullptr);

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
const XMLCh* XStr::begin () const
{
  return this->wstr_;
}

//
// end
//
const XMLCh* XStr::end () const
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
    static_cast<XMLCh *> (XMLPlatformUtils::fgMemoryManager->allocate (
      length * sizeof (XMLCh)));

  bool bOK = result != nullptr;

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

    if (this->cstr_ != nullptr)
    {
      XMLString::release (&this->cstr_);
      this->cstr_ = nullptr;
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
      const std::size_t length = size () - (tail - head) + 1;
      XMLCh *result =
        static_cast<XMLCh *> (xercesc::XMLPlatformUtils::fgMemoryManager->allocate (
          length * sizeof (XMLCh)));
      XMLCh *target = result;
      bOK = target != nullptr;

      if (bOK)
        {
          const XMLCh *cursor = begin();

          while (cursor != head) *target++ = *cursor++;
          cursor = tail;
          while ( cursor != end() ) *target++ = *cursor++;
          *target ++ = 0;

          XMLString::release (&this->wstr_);
          this->wstr_ = result;

          if (this->cstr_ != nullptr)
          {
            XMLString::release (&this->cstr_);
            this->cstr_ = nullptr;
          }
        }
    }
  return bOK;
}

//
// size
//
int XStr::size () const
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
XMLCh * XStr::release ()
{
  XMLCh * temp = this->wstr_;
  this->wstr_ = nullptr;

  // Release the C string.
  if (this->cstr_ != nullptr)
  {
    XMLString::release (&this->cstr_);
    this->cstr_ = nullptr;
  }

  return temp;
}

//
// c_str
//
const char * XStr::c_str ()
{
  if (this->cstr_ != nullptr)
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
