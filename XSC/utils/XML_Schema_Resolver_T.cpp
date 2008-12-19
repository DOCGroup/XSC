// $Id$
#ifndef XML_SCHEMA_RESOLVER_TPP
#define XML_SCHEMA_RESOLVER_TPP

namespace XSC
{
  namespace XML
  {
    template<typename Resolver>
    XML_Schema_Resolver<Resolver>::XML_Schema_Resolver (void)
      : resolver_ ()
    {
    }

    template<typename Resolver>
    XML_Schema_Resolver<Resolver>::XML_Schema_Resolver (Resolver &res)
      : resolver_ (res)
    {
    }

    /// This function is called by the Xerces infrastructure to
    /// actually resolve the location of a schema.
    template<typename Resolver>
    InputSource *
    XML_Schema_Resolver<Resolver>::resolveEntity (const XMLCh *const publicId,
                                                  const XMLCh *const systemId)
    {
      return this->resolver_ (publicId, systemId);
      //XStr path = resolver_ (publicId, systemId);
      //if (path.begin () == 0)
      //  return 0;

      //// Ownership of these objects is given to other people.
      //return new LocalFileInputSource (path);
    }
  }
}

#endif /*XML_SCHEMA_RESOLVER_TPP*/
