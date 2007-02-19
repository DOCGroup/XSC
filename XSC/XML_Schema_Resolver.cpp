// $Id$
#include "XML_Schema_Resolver.hpp"

#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

using xercesc::Wrapper4InputSource;
using xercesc::LocalFileInputSource;

#include "XercesString.hpp"

using xercesc::XMLString;

#include <iostream>

namespace XSC
{
  Schema_Resolver::Schema_Resolver (const Paths &paths)
  {
    paths_.resize (paths.size ());
    std::copy (paths.begin (),
               paths.end (),
               paths_.begin ());
  }

  Schema_Resolver::Schema_Resolver (void)
  {
    paths_.push_back (fs::path ("./"));
  }
  
  
  struct FileLocatorPredicateFoo
  {
    FileLocatorPredicateFoo ( char *file)
      : file_ (file)
    {
    }
    
    bool operator () (fs::path const &x)
    {
      try
        {
          return false;
        }
      catch (...)
        {
          return false;
        }
    }

    const char *file_;
  };
  
  /// This function is called by the Xerces infrastructure to
  /// actually resolve the location of a schema.
  DOMInputSource *
  Schema_Resolver::resolveEntity (const XMLCh *const publicId,
                                  const XMLCh *const systemId,
                                  const XMLCh *const baseURI)
  {
    char *str = XMLString::transcode (systemId);
    
    FileLocatorPredicateFoo pred (str);
    
    fs::path file_path;

    for (Paths::const_iterator i = paths_.begin ();
         i != paths_.end (); ++i)
      {
        fs::path temp (*i / str);
        
        if (fs::exists (temp))
          {
            file_path /= temp;
            break;
          } 
      }
      
    XMLString::release (&str);
    
    XStr path (file_path.string ().c_str ());
    //path.append (systemId);
    
    return new Wrapper4InputSource (new LocalFileInputSource (path));
  }
}
