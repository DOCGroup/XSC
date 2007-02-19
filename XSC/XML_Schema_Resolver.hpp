/**
 * @file XML_Schema_Resolver.h
 * @author Will Otte <wotte@dre.vanderbilt.edu>
 *
 * $Id$
 *
 * Resolves schema locations.
 */

#ifndef CIAO_CONFIG_HANDLERS_XML_SCHEAM_RESOLVER_H
#define CIAO_CONFIG_HANDLERS_XML_SCHEAM_RESOLVER_H

#include <xercesc/dom/DOMEntityResolver.hpp>
#include <xercesc/dom/DOMInputSource.hpp>

#include <CCF/CompilerElements/FileSystem.hpp>

#include <string>
#include <vector>

using xercesc::DOMEntityResolver;
using xercesc::DOMInputSource;
using std::vector;

namespace XSC
{
  /**
   * @class Schema_Resolver
   * @brief Resolves schema locations for CIAO.
   */
  class Schema_Resolver
    : public virtual DOMEntityResolver
  {
  public:
    typedef vector<fs::path> Paths;

    Schema_Resolver (const Paths &paths);

    Schema_Resolver (void);

    /// This function is called by the Xerces infrastructure to
    /// actually resolve the location of a schema.
    virtual DOMInputSource * resolveEntity (const XMLCh *const publicId,
                                            const XMLCh *const systemId,
                                            const XMLCh *const baseURI);

  private:
    Paths paths_;
  };
}



#endif /*  CIAO_CONFIG_HANDLERS_XML_SCHEAM_RESOLVER_H */
