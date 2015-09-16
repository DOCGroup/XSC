// file      : XSC/Parser.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>



#ifndef XSC_PARSER_HXX
#define XSC_PARSER_HXX

#include <stack>
#include <string>
#include "xercesc/dom/DOM.hpp"
#include "xercesc/util/XMLString.hpp"
#include "XSC/SemanticGraph/Schema.hpp"
#include "CCF/CodeGenerationKit/Regex.hpp"

namespace Xerces = xercesc;

namespace XSC
{
  class string : public std::wstring
  {
  public:
    string ()
    {
    }

    string (wchar_t const* s)
        : std::wstring (s)
    {
    }

    string (std::wstring const& s)
        : std::wstring (s)
    {
    }

    string (size_type n, wchar_t c)
        : std::wstring (n, c)
    {
    }

    string (string s, size_type pos, size_type n)
        : std::wstring (s, pos, n)
    {
    }

    // conversion to bool
    //
  private:
    void
    true_ () const
    {
    }

    typedef void (string::*bool_convertable)() const;

  public:
    operator bool_convertable () const
    {
      return empty () ? 0 : &string::true_;
    }
  };

  namespace XML
  {

    inline
    string
    transcode (XMLCh const* s)
    {
      string r (Xerces::XMLString::stringLen (s), L'0');

      for (std::size_t i (0); *s != XMLCh (0); ++s, ++i)
      {
        r[i] = *s;
      }

      return r;
    }


    inline
    XMLCh*
    transcode (string const& s)
    {
      std::size_t l (s.length ());

      XMLCh* r (new XMLCh[l + 1]);
      XMLCh* ir (r);

      for (std::size_t i (0); i < l; ++ir, ++i)
      {
        *ir = static_cast<XMLCh>(s[i]);
        //std::wcerr << s[i] << "->" << *ir << std::endl;
      }

      *ir = XMLCh (0);

      // std::wcerr << r << std::endl;

      return r;
    }

    class Element
    {
    public:
      Element (Xerces::DOMElement* e)
          : e_ (e),
            name_ (transcode (e->getLocalName ())),
            namespace__ (transcode (e->getNamespaceURI ()))
      {
      }

      string
      name () const
      {
        return name_;
      }

      string
      namespace_ () const
      {
        return namespace__;
      }

    public:
      Element
      parent () const
      {
        return dynamic_cast<Xerces::DOMElement*>(e_->getParentNode ());
      }

    public:
      string
      operator[] (string const& s) const
      {
        XMLCh* name (transcode (s));
        XMLCh const* value (e_->getAttribute (name));
        delete[] name;

        return transcode (value);
      }

    public:
      Xerces::DOMElement*
      dom_element () const
      {
        return e_;
      }

    private:

    private:
      Xerces::DOMElement* e_;

      string name_;
      string namespace__;
    };

    inline
    string
    prefix (string const& n)
    {
      std::size_t i (0);
      while (i < n.length () && n[i] != L':') ++i;

      //std::wcerr << "prefix " << n << " "
      //           << string (n, i == n.length () ? i : 0, i) << std::endl;

      return string (n, i == n.length () ? i : 0, i);
    }

    inline
    string
    uq_name (string const& n)
    {
      std::size_t i (0);
      while (i < n.length () && n[i] != L':') ++i;

      return string (n.c_str () + (i == n.length () ? 0 : i + 1));
    }

    inline
    string
    ns_name (Element const& e, string const& n)
    {
      XMLCh* p (0);

      string wp (prefix (n));

      if (!wp.empty ()) p = transcode (wp);

      XMLCh const* xns (e.dom_element ()->lookupNamespaceURI (p));
      string ns (xns ? transcode (xns) : string (L""));
      delete[] p;
      return ns;
    }

    //@@ can get rid of use_default crap. See XSCRT.
    //
    inline
    string
    ns_prefix (Element const& e,
               string const& ns,
               bool
#if XERCES_VERSION_MAJOR != 3
                    use_default
#endif
               )
    {
      XMLCh* p (transcode (ns));

#if XERCES_VERSION_MAJOR == 3
      XMLCh const* xns (e.dom_element ()->lookupPrefix (p));
#else
      XMLCh const* xns (e.dom_element ()->lookupNamespacePrefix (p, use_default));
#endif
      string prefix (xns ? transcode (xns) : string (L""));
      delete[] p;
      return prefix;
    }

    inline
    string
    fq_name (Element const& e, string const& n)
    {
      string ns (ns_name (e, n));
      string un (uq_name (n));

      return ns.empty () ? un : string (ns + L'#' + un);
    }
  }

  class Parser
  {
  public:

    Parser (bool trace, const std::vector <fs::path> &include_paths);

    SemanticGraph::Schema*
    parse (fs::path const&);

  private:
    Xerces::DOMDocument*
    dom (fs::path const&);

    void
    schema (XML::Element const&);

    /// Skips a single annotation element, if present.
    void
    annotation ();

    void
    import (XML::Element const&);

    void
    include (XML::Element const&);

    void
    group (XML::Element const&);

    SemanticGraph::Type*
    simple_type (XML::Element const&);

    SemanticGraph::Type*
    simple_content_restriction (XML::Element const&);

    SemanticGraph::Type*
    complex_content_restriction (XML::Element const&);

    void
    enumeration (XML::Element const&);

    SemanticGraph::Type*
    complex_type (XML::Element const &, const string & type_name = L"");

    void
    all (XML::Element const&);

    void
    choice (XML::Element const&);

    void
    sequence (XML::Element const&);

    void
    simple_content (XML::Element const&);

    void
    complex_content (XML::Element const&);

    void
    simple_content_extension (XML::Element const&);

    void
    complex_content_extension (XML::Element const&);

    void
    element (XML::Element const&, bool global = false);

    void
    attribute (XML::Element const&, bool global = false);

    void
    attribute_group (XML::Element const&, bool global = false);

  private:
    /// @returns true if there are more children of current parent
    bool
    more () const
    {
      Iterator const& it (iteration_state_.top ());

      return it.l_->getLength () > it.i_;
    }

    /// Go to next child of current parent and return it.
    XML::Element
    next ()
    {
      Iterator& it (iteration_state_.top ());

      return XML::Element (
        dynamic_cast<Xerces::DOMElement*> (it.l_->item (it.i_++)));
    }

    /// Go to previous child of current parent
    void
    prev ()
    {
      Iterator& it (iteration_state_.top ());

      if (it.i_) --it.i_;
    }

    /// Places a new parent element on the stack.  Further calls to
    /// more, next, and prev will use this element.
    void
    push (XML::Element const& e)
    {
      iteration_state_.push (e.dom_element ());
    }

    /// Removes the current parent element from the stack.
    void
    pop ()
    {
      iteration_state_.pop ();
    }

  private:
    void
    push_scope (SemanticGraph::Scope& s)
    {
      scope_stack_.push (&s);
    }

    void
    pop_scope ()
    {
      scope_stack_.pop ();
    }

    SemanticGraph::Scope&
    scope () const
    {
      return *(scope_stack_.top ());
    }

  private:
    void
    push_cardinality (unsigned long min, unsigned long max)
    {
      cardinality_stack_.push (Cardinality (min, max));
    }

    void
    pop_cardinality ()
    {
      cardinality_stack_.pop ();
    }

    unsigned long
    min () const
    {
      if (cardinality_stack_.empty ()) return 1;
      else return cardinality_stack_.top ().min_;
    }

    unsigned long
    max () const
    {
      if (cardinality_stack_.empty ()) return 1;
      else return cardinality_stack_.top ().max_;
    }

    string
    normalize (const string &str)
    {
      string retval (str);
      size_t pos (string::npos);

      while ((pos = retval.find_first_of (L":")) != string::npos)
        {
          retval[pos] = L'_';
        }

      return retval;
    }

  private:
    template <typename Edge, typename Node>
    void
    set_type (string const& type, XML::Element const& e, Node& node);

  private:
    struct Iterator
    {
      Iterator (Xerces::DOMElement* e)
          : l_ (e->getChildNodes ()), i_ (0)
      {
      }

      Xerces::DOMNodeList* l_;
      unsigned long i_;
    };

    std::stack<Iterator> iteration_state_;
    SemanticGraph::Schema* root_schema_;   // root schema file
    SemanticGraph::Schema* cur_schema_; // current schema file


    //
    //
    //
    std::stack<SemanticGraph::Scope*> scope_stack_;

    //
    //
    //
    struct Cardinality
    {
      Cardinality (unsigned long min, unsigned long max)
          : min_ (min), max_ (max)
      {
      }

      unsigned long min_, max_;
    };

    std::stack<Cardinality> cardinality_stack_;


    //
    //
    //
    struct FilePathComparator
    {
      bool operator () (fs::path const& x, fs::path const& y) const
      {
        return x.string () < y.string ();
      }
    };

    typedef
    std::map<fs::path, string, FilePathComparator>
    FileMap_;

    FileMap_ file_map_;

  public:
    typedef std::vector <fs::path> Paths;

  private:
    Paths include_paths_;

  private:
    bool qualify_attribute_;
    bool qualify_element_;

  private:
    Parser (Parser const&);

    void
    operator= (Parser const&);
  };
}

#endif  // XSC_PARSER_HXX
