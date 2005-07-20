// file      : XSC/Examples/Library/driver.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <iostream>

#include "../dom.hpp"
#include "library.hpp"

using std::wcerr;
using std::endl;

std::wostream&
operator<< (std::wostream& o, std::string const& s)
{
  return o << s.c_str ();
}

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

    Library const& lib = library (doc);

    for (Library::book_const_iterator b (lib.begin_book ());
         b != lib.end_book (); ++b)
    {
      wcerr << endl
            << L"ISBN       : " << b->isbn () << endl
            << L"Title      : " << b->title ()  << endl;

      for (Book::author_const_iterator a (b->begin_author ());
           a != b->end_author ();
           ++a)
      {
        wcerr << L"Author     : " << a->name () << endl;

        if (a->recommends_p () /* @@ VC7.1 && a->recommends () */)
        {
          Book const& rec = static_cast<Book const&> (*(a->recommends ()));
          wcerr << L"Recommends : " << rec.title () << endl;
        }
      }

      for (Book::character_const_iterator c (b->begin_character ());
           c != b->end_character ();
           ++c)
      {
        wcerr << L"Character  : " << c->name () << L" - "
              << c->qualification () << endl;
      }

      wcerr << L"Available  : " << b->available () << endl;
    }

    // Example of traversal usage. Here we are going to dump
    // the list of recommended books.
    //
    {
      wcerr << endl
            << L"Recommended books:" << endl
	    << endl;

      Traversal::Library library;
      Traversal::Book book;
      Traversal::Author author;
      XMLSchema::Traversal::IDREF idref;
      Traversal::Book recommended_book;

      struct T : Lib::Traversal::Title
      {
        virtual void
        traverse (Type const& t)
        {
          wcerr << t << endl;
        }
      } title;


      // library->book
      //
      library.traverser (book);

      // book->author
      //
      book.traverser (author);

      // author->idref
      //
      author.traverser (idref);

      // idref->recommended_book
      //
      idref.traverser (recommended_book);

      // recommended_book->title
      //
      recommended_book.traverser (title);

      library.dispatch (lib);
    }
  }
}
