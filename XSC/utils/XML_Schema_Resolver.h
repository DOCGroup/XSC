/**
 * @file XML_Schema_Resolver.h
 * @author Will Otte <wotte@dre.vanderbilt.edu>
 *
 * $Id$
 *
 * Resolves schema locations.
 */

#ifndef CIAO_XML_SCHEMA_RESOLVER_H
#define CIAO_XML_SCHEMA_RESOLVER_H

#include "XSC_Utils_export.h"
#include "XercesString.h"
#include "XML_Error_Handler.h"
#include "xercesc/sax/EntityResolver.hpp"
#include "ace/ace_wchar.h"
#include <vector>
#include <string>

using namespace xercesc;

namespace XSC
{
  namespace XML
  {
    // forward decl.
    struct NoOp_Resolver;

    /**
     * @class CIAO_Schema_Resolver
     * @brief Resolves schema locations for CIAO.
     *
     * Template argument Resolver should be a functor with an operation
     * const char * operator () (...arguments from resolveEntity...)
     */
    template <typename Resolver = NoOp_Resolver>
    class XML_Schema_Resolver
      : public virtual EntityResolver
    {
    public:
      XML_Schema_Resolver (Resolver &resolver);

      /// This function is called by the Xerces infrastructure to
      /// actually resolve the location of a schema.
      virtual InputSource * resolveEntity (const XMLCh *const publicId,
                                           const XMLCh *const systemId);

    private:
      XML_Schema_Resolver (void);
      XML_Schema_Resolver (XML_Schema_Resolver<Resolver> &);

      Resolver &resolver_;
    };

    /**
     * @class NoOp_Resolver
     * @brief Resolver that does nothing.
     */
    struct NoOp_Resolver
    {
      InputSource * operator() (const XMLCh *const,
                                const XMLCh *const) const
      { return 0; };
    };

    /**
     * @class Basic_Resolver
     * @brief Resolves a schema location from a fixed path.
     */
    template <typename CHAR = char>
    struct Basic_Resolver
    {
      /// Type definition of the char type.
      typedef CHAR char_type;

      /// Defualt consturctor.
      Basic_Resolver (void);

      /**
       * Initailizing constructor
       *
       * @param[in]       path          Path for resolving XSD files
       */
      Basic_Resolver (const CHAR * path);

      /**
       * Input source for the schema definition in question
       *
       * @param[in]       publicId      Public Id of the definition
       * @param[in]       systemId      System Id of the definition
       */
      InputSource * operator () (const XMLCh *const publicId,
                                 const XMLCh *const systemId) const;

      /**
       * Update the path of the definitions
       *
       * @param[in]       path          New path
       */
      void path (const CHAR * path);

    private:
      /// Path for resolving schema definitions
      XStr path_;
    };

    /**
     * @struct URL_Resolver
     */
    template <typename CHAR = char>
    struct URL_Resolver
    {
      URL_Resolver (const CHAR *url);

      InputSource * operator() (const XMLCh *const publicId,
                                const XMLCh *const systemId) const;
    private:
      XStr url_;
    };

    /**
     * @struct Path_Resolver
     */
    template <typename CHAR = char>
    struct Path_Resolver
    {
      typedef std::basic_string <CHAR> string_type;

      typedef std::vector <typename string_type> path_type;

      Path_Resolver (void);

      Path_Resolver (const path_type & paths);

      void insert (const CHAR *path);

      InputSource * operator() (const XMLCh *const publicId,
                                const XMLCh *const systemId) const;

    private:
      /// Paths to search for schema definitions.
      path_type paths_;
    };

    /**
     * @class Environment_Resolver
     *
     * Resolves a schema location from a path from an environment variable.
     */
    template <typename CHAR = char>
    struct Environment_Resolver :
      public Path_Resolver <CHAR>
    {
      /**
       * Initializing constructor.
       */
      Environment_Resolver (const CHAR * variable = ACE_TEXT (""),
                            const CHAR * path = ACE_TEXT ("./"));

      void insert (const CHAR * variable,
                   const CHAR * path);
    };
  }
}

#include "XML_Schema_Resolver.cpp"

#endif /*  CIAO_XML_SCHEMA_RESOLVER_H */
