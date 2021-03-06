#include "XML_Helper.h"
#include "XML_Error_Handler.h"
#include "XML_Schema_Resolver.h"
#include "XercesString.h"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
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
    using xercesc::DOMImplementationLS;
    using xercesc::XMLUni;
    using xercesc::DOMDocument;
    using xercesc::DOMException;
    using xercesc::DOMDocumentType;
    using xercesc::XercesDOMParser;
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
            delete message;

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
    XERCES_CPP_NAMESPACE::DOMDocument *
    XML_Helper<Resolver, Error>::create_dom (const char *root,
                                             const char *ns,
                                             DOMDocumentType *doctype) const
    {
      if (root == 0 || ns == 0)
        return 0;

      return this->impl_->createDocument (XStr (ns),
                                          XStr (root),
                                          doctype);
    }

    template <typename Resolver, typename Error>
    XERCES_CPP_NAMESPACE::DOMDocumentType *
    XML_Helper<Resolver, Error>::create_doctype (const char *qn,
                                                 const char *pid,
                                                 const char *sid) const
    {
      return this->impl_->createDocumentType (XStr (qn),
                                              XStr (pid),
                                              XStr (sid));
    }
    template <typename Resolver, typename Error>
    XERCES_CPP_NAMESPACE::DOMDocument *
    XML_Helper<Resolver, Error>::create_dom (const char *url) const
    {

      if (url == 0)
        return 0;

      try
        {
          if (this->parser_.get () == 0)
            this->parser_.reset ((new xercesc::XercesDOMParser ()));

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
            return 0;

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
              delete message;
            }
          return 0;

        }
      catch (const XMLException& e)
        {
          char* message = XMLString::transcode (e.getMessage());
          //          ACE_Auto_Basic_Array_Ptr<char> cleanup_message (message);
          delete message;
          throw 0;
        }
      catch (...)
        {
          /*ACE_DEBUG ((LM_DEBUG,
            "(%P|%t) Caught an unknown exception \n"));*/
          throw;
        }

      return 0;
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
          delete message;

          throw;
        }

      this->initialized_ = false;
      return;
    }

    template <typename Resolver, typename Error>
    bool
    XML_Helper<Resolver, Error>::write_DOM (XERCES_CPP_NAMESPACE::DOMDocument *doc,
                                            const char *file) const
    {
      try
        {
          /*          bool retn;
          std::auto_ptr <XERCES_CPP_NAMESPACE::DOMWriter> writer (impl_->createDOMWriter());

          if (writer->canSetFeature (XMLUni::fgDOMWRTFormatPrettyPrint,
                                     true))
            writer->setFeature (XMLUni::fgDOMWRTFormatPrettyPrint, true);

          std::auto_ptr <xercesc::XMLFormatTarget> ft (new xercesc::LocalFileFormatTarget(file));
          retn = writer->writeNode(ft.get (), *doc);

          return retn;*/
        }
      catch (const xercesc::XMLException &e)
        {
          /**
          char* message =
            XMLString::transcode (e.getMessage());
          ACE_Auto_Basic_Array_Ptr<char> cleanup_message (message);

          char* name =
            XMLString::transcode (e.getType());
          ACE_Auto_Basic_Array_Ptr<char> cleanup_name (name);

          ACE_ERROR ((LM_ERROR, "Caught exception while serializing DOM to file.\n"
                      "Name: %s\n"
                      "Message: %s\n"
                      "SrcFile: %s\n"
                      "SrcLine: %d\n",
                      name,
                      message,
                      e.getSrcFile (),
                      e.getSrcLine ()));
          return false;
          */
        }

      return false;
    }
  }
}
