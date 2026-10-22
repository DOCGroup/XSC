//============================================================
/**
 * @file XML_Helper.h
 *
 * @brief Some helper functions for XML
 *
 * @author Bala Natarajan <bala@dre.vanderbilt.edu>
 * @author William R. Otte <wotte@dre.vanderbilt.edu>
 */
//============================================================
#ifndef CIAO_XML_HELPER_H
#define CIAO_XML_HELPER_H

#include "XML_Error_Handler.h"
#include "XML_Schema_Resolver.h"

#include <memory>

#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"

namespace XSC
{
  namespace XML
  {
    /**
     * @class XML_Helper
     *
     * @brief Helper class for some routine XML stuff.
     */
    template <typename Resolver = XML_Schema_Resolver <>,
              typename Error_Handler = XML_Error_Handler>
    class XML_Helper
    {
    public:
      //      XML_Helper (void);

      // Initialize the resolver we should use.
      XML_Helper (Resolver &resolver, Error_Handler &eh);

      ~XML_Helper (void);

      /// Create a DOM tree
      xercesc::DOMDocument *
      create_dom (const char *uri) const;

      xercesc::DOMDocument *
      create_dom (const char *root,
                  const char *ns,
                  xercesc::DOMDocumentType * doctype = nullptr) const;

      xercesc::DOMDocumentType *
      create_doctype (const char *qn,
                      const char *pid,
                      const char *sid) const;

      //Writes out a DOMDocument to an XML file
      bool write_DOM (xercesc::DOMDocument *doc,
                      const char *file) const;

      bool is_initialized (void) const;

    protected:
      /// Intialize the parser
      void init_parser (void);

      /// Terminate the parser
      void terminate_parser (void);

    private:
      XML_Helper (XML_Helper const&) = delete;
      XML_Helper& operator= (XML_Helper const&) = delete;
      XML_Helper (XML_Helper&&) = delete;
      XML_Helper& operator= (XML_Helper&&) = delete;

      bool initialized_;
      xercesc::DOMImplementation *impl_;
      mutable std::unique_ptr<xercesc::XercesDOMParser> parser_;

      Resolver &resolver_;
      Error_Handler &e_handler_;
    };


  }
}

#include "XML_Helper_T.cpp"

#endif/*CIAO_CONFIG_HANDLERS_XML_HELPER_H*/
