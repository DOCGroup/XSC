// $Id$

#include "XML_Schema_Resolver.h"
#include "XercesString.h"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
#include <iostream>

namespace XSC
{
  namespace XML
  {
    Basic_Resolver::Basic_Resolver (const char *path)
      : path_ (path)
    {
    }

    XMLCh *
    Basic_Resolver::operator() (const XMLCh *const,
                                const XMLCh *const systemId) const
    {
      XStr path (path_);
      path.append (systemId);
      return path.release ();
    }

    Environment_Resolver::Environment_Resolver (const char *variable,
                                                const char *relpath)
    {
      xercesc::XMLPlatformUtils::Initialize();
      this->add_path (variable, relpath);
    }

    using xercesc::XMLPlatformUtils;

    void
    Environment_Resolver::add_path (const char *variable,
                                    const char *relpath)
    {/*
      ACE_Env_Value <const char *> path_env (variable,
                                                  ACE_TEXT(""));

      XStr xpath (path_env);
      XStr xrelpath (relpath);

      xpath.append (xrelpath);

      paths_.push_back (xpath);*/
    }

    XMLCh *
    Environment_Resolver::operator() (const XMLCh *const,
                                      const XMLCh *const systemId) const
    {
      for (std::vector<XStr>::const_iterator i = this->paths_.begin ();
           i != this->paths_.end ();
           ++i)
        {
          XStr path (*i);
          path.append(systemId);

          xercesc::FileHandle file (XMLPlatformUtils::openFile (path));

          if (file != 0)
            {
              XMLPlatformUtils::closeFile (file);
              return path.release ();
            }
        }
      return 0;
    }
  }
}
