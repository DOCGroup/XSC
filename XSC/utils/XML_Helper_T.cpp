#include "XML_Helper.h"
#include "XML_Error_Handler.h"
#include "XML_Schema_Resolver.h"
#include "XercesString.h"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/util/XMLUniDefs.hpp"

namespace XSC
{
  namespace XML
  {
    using xercesc::XMLException;
    using xercesc::XMLString;
    using xercesc::DOMImplementation;
    using xercesc::DOMImplementationRegistry;
//    using xercesc::DOMBuilder;
    using xercesc::DOMDocument;
    using xercesc::DOMException;
    using xercesc::DOMDocumentType;
/*
    template <typename Resolver, typename Error>
    XML_Helper<Resolver, Error>::XML_Helper (void)
      : initialized_ (false)
    {
      this->init_parser ();
    }
*/
    // TODO this is stub implementation
    template <typename Resolver, typename Error>
    XML_Helper<Resolver, Error>::XML_Helper (Resolver &resolver, Error &eh)
      : initialized_ (false),
        resolver_ (resolver),
        e_handler_ (eh)
    {
      this->init_parser ();
    }

    template <typename Resolver, typename Error>
    XML_Helper<Resolver, Error>::~XML_Helper (void)
    {
      this->terminate_parser ();
    }

    template <typename Resolver, typename Error>
    bool
    XML_Helper<Resolver, Error>::is_initialized (void) const
    {
      return this->initialized_ == true;
    }

    template <typename Resolver, typename Error>
    void
    XML_Helper<Resolver, Error>::init_parser (void)
    {
      if (this->initialized_)
        return;

      /*ACE_DEBUG ((LM_DEBUG,
        "(%P|%t) Initializing the Xerces runtime \n"));*/
      // Initialize the Xerces run-time
      try
        {
          xercesc::XMLPlatformUtils::Initialize();
        }
      catch (const XMLException& e)
        {
          char* message =
            XMLString::transcode (e.getMessage());
            //          ACE_Auto_Basic_Array_Ptr<char> cleanup_message (message);
            XMLString::release (&message);

          throw;
        }
      catch (...)
        {
          /*ACE_DEBUG ((LM_DEBUG,
                      "(%P|%t) Some other exception,"
                      " returning\n"));*/

          return;
        }

      // Instantiate the DOM parser.
      static const XMLCh gLS[] = { xercesc::chLatin_L,
                                   xercesc::chLatin_S,
                                   xercesc::chNull };

      // Get an implementation of the Load-Store (LS) interface
      // and cache it for later use
      impl_ =
        DOMImplementationRegistry::getDOMImplementation(gLS);

      this->initialized_ = true;
      return;
    }

    template <typename Resolver, typename Error>
    xercesc::DOMDocument *
    XML_Helper<Resolver, Error>::create_dom (const char *root,
                                             const char *ns,
                                             DOMDocumentType *doctype) const
    {
      if (root == nullptr || ns == nullptr)
        return nullptr;

      return this->impl_->createDocument (XStr (ns),
                                          XStr (root),
                                          doctype);
    }

    template <typename Resolver, typename Error>
    xercesc::DOMDocumentType *
    XML_Helper<Resolver, Error>::create_doctype (const char *qn,
                                                 const char *pid,
                                                 const char *sid) const
    {
      return this->impl_->createDocumentType (XStr (qn),
                                              XStr (pid),
                                              XStr (sid));
    }
    template <typename Resolver, typename Error>
    xercesc::DOMDocument *
    XML_Helper<Resolver, Error>::create_dom (const char *url) const
    {

      if (url == nullptr)
        return nullptr;

      try
        {
          if (this->parser_.get () == nullptr)
            this->parser_ = std::make_unique<xercesc::XercesDOMParser> ();

          // Perform Namespace processing.
          this->parser_->setDoNamespaces (true);

          // Discard comment nodes in the document
          this->parser_->setCreateCommentNodes (false);

          // Disable datatype normalization. The XML 1.0 attribute value
          // normalization always occurs though.
          // this->parser_->setFeature (XMLUni::fgDOMDatatypeNormalization, true);

          // Do not create EntityReference nodes in the DOM tree. No
          // EntityReference nodes will be created, only the nodes
          // corresponding to their fully expanded sustitution text will be
          // created.
          this->parser_->setCreateEntityReferenceNodes (false);

          // Perform Validation
          this->parser_->setValidationScheme (xercesc::AbstractDOMParser::Val_Always);

          // Do not include ignorable whitespace in the DOM tree.
          this->parser_->setIncludeIgnorableWhitespace (false);

          // Enable the parser's schema support.
          this->parser_->setDoSchema (true);

          // Enable full schema constraint checking, including checking which
          // may be time-consuming or memory intensive. Currently, particle
          // unique attribution constraint checking and particle derivation
          // restriction checking are controlled by this option.
          this->parser_->setValidationSchemaFullChecking (true);

          // The parser will treat validation error as fatal and will exit.
          this->parser_->setValidationConstraintFatal (true);

          this->parser_->setErrorHandler (&e_handler_);

          this->parser_->setEntityResolver (&resolver_);

          this->parser_->parse (url);

          if (e_handler_.getErrors ())
            return nullptr;

          return this->parser_->adoptDocument ();
        }
      catch (const DOMException& e)
        {
          const unsigned int maxChars = 2047;
          XMLCh errText[maxChars + 1];

          if (DOMImplementation::loadDOMExceptionMsg (e.code,
                                                      errText,
                                                      maxChars))
            {
              char* message =
                XMLString::transcode (errText);
//                              ACE_Auto_Basic_Array_Ptr<char> cleanup_message (message);
              XMLString::release (&message);
            }
          return nullptr;

        }
      catch (const XMLException& e)
        {
          char* message = XMLString::transcode (e.getMessage());
          //          ACE_Auto_Basic_Array_Ptr<char> cleanup_message (message);
          XMLString::release (&message);
          throw;
        }
      catch (...)
        {
          /*ACE_DEBUG ((LM_DEBUG,
            "(%P|%t) Caught an unknown exception \n"));*/
          throw;
        }

      return nullptr;
    }

    template <typename Resolver, typename Error>
    void
    XML_Helper<Resolver, Error>::terminate_parser (void)
    {
      try
        {
          xercesc::XMLPlatformUtils::Terminate();
        }
      catch (const XMLException& e)
        {
          char* message =
            XMLString::transcode (e.getMessage());
          //ACE_Auto_Basic_Array_Ptr<char> cleanup_message (message);
          XMLString::release (&message);

          throw;
        }

      this->initialized_ = false;
      return;
    }

    template <typename Resolver, typename Error>
    bool
    XML_Helper<Resolver, Error>::write_DOM (xercesc::DOMDocument *doc,
                                            const char *file) const
    {
      (void) doc;
      (void) file;
      return false;
    }
  }
}
