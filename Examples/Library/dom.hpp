// Xerces DOoM ;-).
//
//

#include <xercesc/dom/DOM.hpp>

class ErrorHandler : public  xercesc::DOMErrorHandler
{
public:
  ErrorHandler ();

  virtual bool
  handleError (xercesc::DOMError const& e);

  bool
  failed () const;

private:
  bool failed_;
};

xercesc::DOMDocument*
dom (char const* name);
