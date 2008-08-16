// $Id$

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

namespace XSCRT
{
  namespace utils
  {
    ///////////////////////////////////////////////////////////////////////////
    // XML_Schema_Resolver_T

    template <typename Resolver>
    XML_Schema_Resolver_T <Resolver>::
    XML_Schema_Resolver_T (const Resolver & res)
      : resolver_ (res)
    {
    }

    /// This function is called by the Xerces infrastructure to
    /// actually resolve the location of a schema.
    template <typename Resolver>
    xercesc::DOMInputSource *
    XML_Schema_Resolver_T <Resolver>::
    resolveEntity (const XMLCh * const publicId,
                   const XMLCh * const systemId,
                   const XMLCh * const baseURI)
    {
      XSC::XStr path = this->resolver_ (publicId, systemId, baseURI);

      if (path.begin () == 0)
        return 0;

      // Ownership of these objects is given to other people.
      return new xercesc::
        Wrapper4InputSource (new xercesc::LocalFileInputSource (path));
    }

    ///////////////////////////////////////////////////////////////////////////
    // Basic_Resolver_T

    template <typename T>
    Basic_Resolver_T <T>::Basic_Resolver_T (const T * path)
      : path_ (path)
    {
    }

    template <typename T>
    Basic_Resolver_T <T>::Basic_Resolver_T (const Basic_Resolver_T <T> & res)
      : path_ (res.path_)
    {
    }

    template <typename T>
    XMLCh * Basic_Resolver_T <T>::operator () (const XMLCh * const,
                                               const XMLCh * const systemId,
                                               const XMLCh * const) const
    {
      XSC::XStr path (this->path_);
      path.append (systemId);

      XMLCh * retval = path.release ();
      return retval;
    }

    ///////////////////////////////////////////////////////////////////////////
    // xml_schema_resolver

    template <typename T>
    XML_Schema_Resolver_T <T> * xml_schema_resolver (const T & resolver)
    {
      return new XML_Schema_Resolver_T <T> (resolver);
    }
  }
}
