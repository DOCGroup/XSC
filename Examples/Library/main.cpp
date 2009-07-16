//-------------------------------------------------------//
//Author:  Jason R. Cody                                 //
//Class: CS390-XML Schema Compiler                       //
//Date Modified: 30JUN09                                 //
//-------------------------------------------------------//
//Notes:                                                 //
//  -30JUN09:  Recreated new main.cpp to simplify the    //
//   initialization of the Xerces parser.                //
//  -01JUL09:  Added elements from Library\Reader\       //
//   driver.cpp.                                         //
//-------------------------------------------------------//

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

//Xerces includes (needed to create a parser).
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>

#include "library.hpp"

std::wostream&
operator << (std::wostream& o, std::string const& s)
{
  return o << s.c_str();
}


void Xerces_Interface (std::string const funct)
{
  if (funct == "init")
  {
    try 
    {
      xercesc::XMLPlatformUtils::Initialize();
      std::cout << "\n Xerces Initialized.\n";
    }
    catch (const xercesc::XMLException& toCatch)
    {
      std::cout << "XML Exception raised." << std::endl;
    }
  } else if (funct == "terminate")
  {
    xercesc::XMLPlatformUtils::Terminate();
    std::cout << "\n Xerces Terminated.\n";
  }
}

//Gets a document in the form of a string from 
//the user.
void get_doc_name (std::string &xml_document)
{
    std::cout << "\nEnter XML document: ";
    std::cin >> xml_document;
    getchar();
}

//Returns a buffer from a given file name.
//Note: Should throw an exception if a file isn't
//  opened (currently allows the buff to not be 
//  initialized)
void get_buff (std::string file_name, char * &buffer)
{
  std::ifstream file_handle;
  file_handle.open(file_name.c_str(), ios::binary);

  if (!file_handle.is_open())
  {
    fstream::failure fail("FILE NOT FOUND");
    throw fail;
  }
  file_handle.seekg(0, ios::end);
  int length = file_handle.tellg();
  file_handle.seekg(0, ios::beg);

  buffer = new char [length];

  file_handle.read(buffer, length);
  file_handle.close();
}

int main (int argc, char *argv[])
{
    //Name of the xml_document and file handler
    std::string xml_document;
    std::ifstream xml_handle;

    //Buffer that will store the contents of the
    //xml file.
    char *buffer;

  try
  {
    
    //Initialize the Xerces Interface
    Xerces_Interface("init");
    
    //Create the Xerces Parser
    xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser();

    //NOTE-----------------------//
    //Here a default handler is  //
    //used, but a real one should//
    //be written in the future.  //
    //---------------------------//
    xercesc::HandlerBase handler;

    //Set up the parser for the parsing.

    parser->setEntityResolver(&handler);
    parser->setCreateCommentNodes(false);
    parser->setCreateEntityReferenceNodes(false);
    parser->setDoNamespaces(true);
    parser->setIncludeIgnorableWhitespace(false);
    parser->setValidationScheme(xercesc::AbstractDOMParser::Val_Auto);
    parser->setDoSchema(true);
    parser->setValidationSchemaFullChecking(true);
    parser->setValidationConstraintFatal(false);
    
    //Create the DOMDocument that will hold the tree.
    xercesc::DOMDocument *doc = 0;

    //Get the xml file name and store the buffer.
    get_doc_name(xml_document);
    get_buff(xml_document, buffer);

    //Parse the file in xml_document and store the 
    //DOMDocument in doc.
    parser->parse(xml_document.c_str()); 
    doc = parser->getDocument();

    Lib::Library const& my_library = Lib::reader::library(doc);

    for (Lib::Library::book_const_iterator b (my_library.begin_book());
         b != my_library.end_book(); ++b)
    {
       std::wcerr << std::endl
                  << L"ISBN      : " << (*b).get()->isbn() << std::endl;
       std::wcerr << std::endl
                  << L"TITLE     : " << (*b).get()->title() << std::endl;
       for (Lib::Book::author_const_iterator a ((*b).get()->begin_author());
            a != (*b).get()->end_author();
            ++a)
       {
         std::wcerr << std::endl
                    << L"Author     : " << (*a).get()->name() << std::endl;
         if ((*a).get()->recommends_p())
         {
           std::wcerr << L"Recommends ";
           Lib::Book *temp_book = 
                     dynamic_cast<Lib::Book*> ((*a).get()->recommends_ptr());
           std::wcerr << temp_book->title() << std::endl;
           std::wcerr << L"Recommends ID: " << ((*a).get()->recommends()).id() << std::endl;
         }
       }

       for (Lib::Book::character_const_iterator c ((*b).get()->begin_character ());
            c != (*b).get()->end_character ();
            ++c)
       {
         std::wcerr << L"Character     : " << (*c).get()->name() << L" - "
                    << (*c).get()->qualification() << std::endl;
       }
      std::wcerr << L"Available     : " << (*b).get()->available() << std::endl;
    }


    size_t count = my_library.count_book();

    std::cout << "Number of Books is: " << count << std::endl;

    size_t i = 1;

    delete [] buffer;
    Xerces_Interface("terminate");
    return 0;
  }
  catch (fstream::failure)
  {
    std::cout << "Failed to open: " << xml_document << std::endl;
    return -1;
  }
  catch (const xercesc::XMLException &toCatch)
  {
    char* message = xercesc::XMLString::transcode(toCatch.getMessage());
    std::cout << "Exception message is: \n" << message << std::endl;
    xercesc::XMLString::release(&message);
    return -1;
  }
  catch (...)
  {
    std::cout << "Unexpected Exception Occurred." << std::endl;
  }

}
