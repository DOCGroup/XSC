// $Id$
#ifndef XML_SCHEMA_RESOLVER_TPP
#define XML_SCHEMA_RESOLVER_TPP

namespace XSC
{
  namespace XML
  {
    //template<typename Resolver>
    //XML_Schema_Resolver<Resolver>::XML_Schema_Resolver (void)
    //  : resolver_ ()
    //{
    //}

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
    }
  }
}

#endif /*XML_SCHEMA_RESOLVER_TPP*/
