// $Id$
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

    template <typename CHAR>
    Path_Resolver <CHAR>::Path_Resolver (void)
    {

    }

    template <typename CHAR>
    Path_Resolver <CHAR>::Path_Resolver (const path_type & paths)
    {
      for (typename path_type::const_iterator iter = paths.begin ();
           iter != paths.end ();
           ++ iter)
      {
        this->paths_.push_back (iter->c_str ());
      }
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
      for (std::vector <XStr>::const_iterator i = this->paths_.begin ();
           i != this->paths_.end ();
           ++i)
        {
          XStr path (*i);
          path.append(systemId);

          FileHandle file (XMLPlatformUtils::openFile (path));

          if (file != 0)
            {
              XMLPlatformUtils::closeFile (file);
              return new xercesc::LocalFileInputSource (path);
            }
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
