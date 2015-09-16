#ifndef XML_SCHEMA_RESOLVER_TPP
#define XML_SCHEMA_RESOLVER_TPP

#include "XercesString.h"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/URLInputSource.hpp"
#include "ace/Env_Value_T.h"

namespace XSC
{
  namespace XML
  {
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

    ///////////////////////////////////////////////////////////////////////////
    // class Basic_Resolver

    template <typename CHAR>
    Basic_Resolver <CHAR>::Basic_Resolver (void)
    {
    }

    template <typename CHAR>
    Basic_Resolver <CHAR>::Basic_Resolver (const CHAR *path)
      : path_ (path)
    {
    }

    template <typename CHAR>
    void Basic_Resolver <CHAR>::path (const CHAR *path)
    {
      this->path_ = path;
    }

    template <typename CHAR>
    InputSource *
    Basic_Resolver <CHAR>::operator() (const XMLCh *const publicId,
                                       const XMLCh *const systemId) const
    {
      XStr path (path_);
      path.append (systemId);

      return new xercesc::LocalFileInputSource (path);
    }

    ///////////////////////////////////////////////////////////////////////////
    // class Path_Resolver

    struct Path_Resolver_Functor
    {
      Path_Resolver_Functor (const XMLCh *const systemId)
        : systemId_ (systemId)
      {

      }

      template <typename T>
      bool operator () (T item)
      {
        if (item.empty ())
          return false;

        XStr path (item.c_str ());
        path.append (this->systemId_);

        FileHandle file (XMLPlatformUtils::openFile (path));
        bool retval = 0 != file ? true : false;

        if (retval)
          XMLPlatformUtils::closeFile (file);

        return retval;
      }

    private:
      const XMLCh *const systemId_;
    };

    template <typename CHAR>
    Path_Resolver <CHAR>::Path_Resolver (void)
    {

    }

    template <typename CHAR>
    Path_Resolver <CHAR>::Path_Resolver (const path_type & paths)
    : paths_ (paths)
    {

    }

    template <typename CHAR>
    void Path_Resolver <CHAR>::insert (const CHAR * path)
    {
      this->paths_.push_back (path);
    }

    template <typename CHAR>
    InputSource *
    Path_Resolver <CHAR>::operator() (const XMLCh *const,
                                      const XMLCh *const systemId) const
    {
      typename path_type::const_iterator iter =
        std::find_if (this->paths_.begin (),
                      this->paths_.end (),
                      Path_Resolver_Functor (systemId));

      if (iter != this->paths_.end ())
      {
        XStr path (iter->c_str ());
        path.append (systemId);
        return new xercesc::LocalFileInputSource (path);
      }

      return 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    // class Environment_Resolver

    template <typename CHAR>
    Environment_Resolver <CHAR>::
    Environment_Resolver (const CHAR *variable, const CHAR *relpath)
    {
      this->insert (variable, relpath);
    }

    template <typename CHAR>
    void Environment_Resolver <CHAR>::insert (const CHAR *variable,
                                              const CHAR *relpath)
    {
      ACE_Env_Value <const CHAR *> path (variable, "");

      std::basic_string <CHAR> xrelpath =
        std::basic_string <CHAR> (path) +
        std::basic_string <CHAR> (relpath);

      Path_Resolver <CHAR>::insert (xrelpath.c_str ());
    }

    ///////////////////////////////////////////////////////////////////////////
    // class URL_Resolver

    template <typename CHAR>
    URL_Resolver <CHAR>::URL_Resolver (const CHAR * url)
      : url_ (url)
    {
    }

    template <typename CHAR>
    InputSource * URL_Resolver <CHAR>::
    operator() (const XMLCh *const publicId, const XMLCh *const systemId) const
    {
      xercesc::XMLURL url (this->url_, systemId);
      return new xercesc::URLInputSource (url);
    }
  }
}

#endif /*XML_SCHEMA_RESOLVER_TPP*/
