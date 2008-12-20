// $Id$

#include "XML_Schema_Resolver.h"
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
    ///////////////////////////////////////////////////////////////////////////
    // class Basic_Resolver

    Basic_Resolver::Basic_Resolver (void)
    {
    }

    Basic_Resolver::Basic_Resolver (const char *path)
      : path_ (path)
    {
    }

    void Basic_Resolver::path (const char *path)
    {
      this->path_ = path;
    }

    InputSource *
    Basic_Resolver::operator() (const XMLCh *const publicId,
                                const XMLCh *const systemId) const
    {
      XStr path (path_);
      path.append (systemId);

      return new xercesc::LocalFileInputSource (path);
    }

    ///////////////////////////////////////////////////////////////////////////
    // class Path_Resolver

    Path_Resolver::Path_Resolver (void)
    {

    }

    Path_Resolver::Path_Resolver (std::vector <std::string> & paths)
    {
      for (std::vector <std::string>::const_iterator iter = paths.begin ();
           iter != paths.end ();
           ++ iter)
      {
        this->paths_.push_back (iter->c_str ());
      }
    }

    void Path_Resolver::insert (const char * path)
    {
      this->paths_.push_back (path);
    }

    InputSource *
    Path_Resolver::operator() (const XMLCh *const,
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

    Environment_Resolver::Environment_Resolver (const char *variable,
                                                const char *relpath)
    {
      this->insert (variable, relpath);
    }

    void
    Environment_Resolver::insert (const char *variable, const char *relpath)
    {
      ACE_Env_Value <const char *> path (variable, "");
      std::string xrelpath = std::string (path) + std::string (relpath);
      Path_Resolver::insert (xrelpath.c_str ());
    }

    ///////////////////////////////////////////////////////////////////////////
    // class URL_Resolver

    URL_Resolver::URL_Resolver (const char * url)
      : url_ (url)
    {
    }

    InputSource * URL_Resolver::operator() (const XMLCh *const publicId,
                                            const XMLCh *const systemId) const
    {
      xercesc::XMLURL url (this->url_, systemId);
      return new xercesc::URLInputSource (url);
    }
  }
}
