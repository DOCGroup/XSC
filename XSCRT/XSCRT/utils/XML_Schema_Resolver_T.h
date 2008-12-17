// -*- C++ -*-

//=============================================================================
/**
 * @file        XML_Schema_Resolver_T.h
 *
 * Resolves schema locations.
 *
 * $Id$
 *
 * @author      Will Otte <wotte@dre.vanderbilt.edu>
 */
//=============================================================================

#ifndef _XML_SCHEMA_RESOLVER_T_H_
#define _XML_SCHEMA_RESOLVER_T_H_

#include "XSCRT/XSCRT_export.h"
#include "XSC/XercesString.hpp"
#include "xercesc/dom/DOMEntityResolver.hpp"
#include "xercesc/dom/DOMInputSource.hpp"
#include <vector>

namespace XSCRT
{
  namespace utils
  {
    // Forward decl.
    struct NoOp_Resolver;

    /**
     * @class XML_Schema_Resolver
     * @brief Resolves schema locations for the target XML file.
     *
     * Template argument Resolver should be a functor with an operation
     * const XMLCh * operator () (...arguments from resolveEntity...)
     */
    template <typename Resolver = NoOp_Resolver>
    class XML_Schema_Resolver_T :
      public virtual xercesc::DOMEntityResolver
    {
    public:
      /**
       * Initialize the XML schema resolver with a predefined
       * resolver object.
       *
       * @param[in]     resolver        The resolver object.
       */
      XML_Schema_Resolver_T (const Resolver & resolver);

      /// This function is called by the Xerces infrastructure to
      /// actually resolve the location of a schema.
      virtual xercesc::DOMInputSource *
      resolveEntity (const XMLCh * const publicId,
                     const XMLCh * const systemId,
                     const XMLCh * const baseURI);

    private:
      /// The actual resolver object.
      Resolver resolver_;

      // prevent the following operation
      XML_Schema_Resolver_T (void);
      XML_Schema_Resolver_T (const XML_Schema_Resolver_T <Resolver> &);
      const XML_Schema_Resolver_T & operator = (const XML_Schema_Resolver_T &);
    };

    /**
     * @class NoOp_Resolver
     * @brief Resolver that does nothing.
     */
    struct XSCRT_Export NoOp_Resolver
    {
      const XMLCh * operator () (const XMLCh * const,
                                 const XMLCh * const systemId,
                                 const XMLCh * const)
      { return systemId; };
    };

    /**
     * @class Basic_Resolver
     * @brief Resolves a schema location from a fixed path.
     */
    template <typename T = char>
    struct Basic_Resolver_T
    {
      /// Type definition of the character type.
      typedef T char_type;

      Basic_Resolver_T (const T * path);

      Basic_Resolver_T (const Basic_Resolver_T & res);

      XMLCh * operator () (const XMLCh * const publicId,
                           const XMLCh * const systemId,
                           const XMLCh * const baseURI) const;

    private:
      XSC::XStr path_;

      // prevent the following operation
      Basic_Resolver_T (void);
    };

    /**
     * @class Basic_Resolver
     * @brief Resolves a schema location from a fixed path.
     */
    template <typename T = char>
    struct Static_Resolver_T
    {
      /// Type definition of the character type.
      typedef T char_type;

      Static_Resolver_T (const T * path);

      Static_Resolver_T (const Static_Resolver_T & res);

      XMLCh * operator () (const XMLCh * const publicId,
                           const XMLCh * const systemId,
                           const XMLCh * const baseURI) const;

    private:
      XSC::XStr path_;

      // prevent the following operation
      Static_Resolver_T (void);
    };

    /**
     * @func  xml_schema_resolver
     * @brief Factory function for creating XML_Schema_Resolver_T
     *        objects.
     */
    template <typename T>
    XML_Schema_Resolver_T <T> * xml_schema_resolver (const T & resolver);
  }
}

#include "XML_Schema_Resolver_T.cpp"

#endif  // !defined _XML_SCHEMA_RESOLVER_T_H_
