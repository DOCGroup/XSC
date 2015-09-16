// file      : XSC/Examples/Library/driver.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include <iostream>

#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include "../dom.hpp"
#include "library.hpp"

using std::wcerr;
using std::endl;

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    wcerr << L"usage: " << argv[0] << L" ../library.xml" << endl;
    return 1;
  }

  if (xercesc::DOMDocument* doc = dom (argv[1]))
  {
    using namespace Lib;

    Library const& src (library (doc));

    // Constructing and writing out the document.
    //
    //
    {
      Library lib (src);

      {
        Book b (ISBN (12345678UL),
                Title (L"Sharper"),
                Genre::fiction,
                true,             // available
                L"sharper");      // id

        Author a (L"Anatoly Barbakaru", L"0000-00-00", L"0000-00-00");

        a.recommends (L"sharper"); // self-promotion

        Character c (L"Author", L"Sharper");

        b.add_author (a);
        b.add_character (c);

        lib.add_book (b);
      }

      using namespace xercesc;
      using namespace XSCRT;

      // Some wrestling with DOM.
      //
      //
      DOMImplementation* impl (
        DOMImplementationRegistry::getDOMImplementation (
          XML::string (L"LS").c_str ()));

      DOMDocument* doc (
        impl->createDocument (
          XML::string (L"http://www.example.com/Lib").c_str (), // root's namespace
          XML::string (L"l:library").c_str (),                  // root element name
          0));                                                  // document type

      DOMElement* root (doc->getDocumentElement ());

      root->setAttributeNS (
        XML::string (L"http://www.w3.org/2000/xmlns/").c_str (),
        XML::string (L"xmlns:xsi").c_str (),
        XML::string (L"http://www.w3.org/2001/XMLSchema-instance").c_str ());

      root->setAttributeNS (
        XML::string (L"http://www.w3.org/2001/XMLSchema-instance").c_str (),
        XML::string (L"xsi:schemaLocation").c_str (),
        XML::string (L"http://www.example.com/Lib library.xsd").c_str ());


      // Actual Lib::Library to DOM serialization.
      //
      library (lib, doc);


      // Some more wrestling with DOM.
      //
      //

      // Get an instance of DOMWriter.
      //
      DOMWriter* writer (((DOMImplementationLS*)impl)->createDOMWriter ());

      // Plug in error handler.
      //
      ErrorHandler eh;
      writer->setErrorHandler(&eh);

      // Set some nice features if the serializer supports them.
      //
      if (writer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
        writer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

      if (writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

      // Plug in a format target to receive the resultant
      // XML stream from the serializer.
      //
      // StdOutFormatTarget prints the resultant XML stream
      // to stdout once it receives anything from the serializer.
      //
      XMLFormatTarget* ft (new LocalFileFormatTarget("out.xml"));

      // Do the serialization through DOMWriter::writeNode();
      //
      writer->writeNode(ft, *doc);


      // I hope I didn't forget anything.
      //
      delete doc;
      delete writer;
      delete ft;
    }
  }
}
