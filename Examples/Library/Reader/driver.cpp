// file      : XSC/Examples/Library/driver.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include <iostream>
#include <type_traits>
#include <utility>

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

    static_assert (std::is_same_v<decltype (std::declval<Library&> ().book_values ()),
                                  Library::book_container_type&>);
    static_assert (std::is_same_v<decltype (std::declval<Library const&> ().book_values ()),
                                  Library::book_container_type const&>);

    Library const& lib = library (doc);

    for (Book const& b : lib.book_values ())
    {
      wcerr << endl
            << L"ISBN       : " << b.isbn () << endl
            << L"Title      : " << b.title ()  << endl;

      for (Author const& a : b.author_values ())
      {
        wcerr << L"Author     : " << a.name () << endl;

        if (a.recommends_p () /* @@ VC7.1 && a.recommends () */)
        {
          Book const& rec = static_cast<Book const&> (*(a.recommends ()));
          wcerr << L"Recommends : " << rec.title () << endl;
        }
      }

      for (Character const& c : b.character_values ())
      {
        wcerr << L"Character  : " << c.name () << L" - "
              << c.qualification () << endl;
      }

      wcerr << L"Available  : " << b.available () << endl;
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
