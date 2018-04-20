// file      : CXX/Header.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "Header.hpp"
#include "Elements.hpp"

#include <string>

#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

#include "CCF/CodeGenerationKit/Regex.hpp"

#if defined (_WINDOWS)
# if defined (min)
#   undef min
# endif
# if defined (max)
#   undef max
# endif
#endif

namespace
{
  struct Base : Traversal::Type, protected virtual Context
  {
    Base (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (SemanticGraph::Type& c)
    {
      string name (type_name (c));

      os << " : public " << name
         << "{"
        //         << "//@@ VC6 anathema" << endl
         << "typedef " << name << " Base;"
         << endl;

    }
  };

  // List of all non-optional members
  //
  //
  struct CtorArgs : Traversal::Complex,
                    protected Traversal::Enumeration,
                    //protected Traversal::Type,
                    protected Traversal::Element,
                    protected Traversal::Attribute,
                    protected virtual Context
  {
    CtorArgs (Context& c)
        : Context (c),
          first_ (false)
    {
      complex_.edge_traverser (inherits_);
      complex_.edge_traverser (names_);

      inherits_.node_traverser (*this);
      names_.node_traverser (*this);
    }

    //virtual void
    //traverse (SemanticGraph::Type& t)
    //{
    //  os << comma () << type_name (t) << " const& " << id (t.name ()) << "__";
    //}

    virtual void
    traverse (SemanticGraph::Enumeration& e)
    {
      os << comma () << type_name (e) << " const& " << e.name () << "__";
    }

    virtual void
    traverse (SemanticGraph::Element& e)
    {
      string type (type_name (e));
      string name (e.name ());

      if (e.min () == 1 && e.max () == 1)
      {
        os << comma () << type << " const& " << name << "__";
      }
      else if (e.min () >= 1)
      {
        os << comma ()
           << name << "_container_type const& "
           << id (name) << "__";
      }
    }

    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      if (!a.optional ())
      {
        os << comma () << type_name (a) << " const& " << id (a.name ()) << "__";
      }
    }

    void
    traverse (SemanticGraph::Complex& c)
    {
      first_ = true;
      complex_.traverse (c);
    }

  private:
    string
    comma ()
    {
      return first_ ? (first_ = false, L"") : L",\n";
    }

    bool first_;

  private:
    Traversal::Complex complex_;

    Traversal::Inherits inherits_;
    Traversal::Names names_;
  };


  struct Element : Traversal::Element, protected virtual Context
  {
    Element (Context& c)
        : Context (c)
    {
    }

    string
    type_name (Type& i)
    {
      SemanticGraph::Type& t (i.type ());

      if (t.named ())
      {
        return Context::type_name (t);
      }
      else
      {
        return anon_prefix_ + id (i.name ()) + anon_suffix_;
      }
    }


    virtual void
    traverse (Type& c)
    {
      string name (c.name ());
      string type (type_name (c));
      string char_type (this->char_type);

      //Check if the type is an IDREF
      size_t idref_ptr = 0;
      std::wstring idref_str (L"::XMLSchema::IDREF<");
      idref_ptr = type.find(idref_str);


      string container;

      if (this->generate_ra_sequences_)
        container = L"std::vector";
      else
        container = L"std::list";

      os << "// " << name << endl
         << "public:" << endl;


      if (c.max () != 1)
      {
        // sequence
        //
        if (this->cpp11_)
        {
          os << "typedef " << container  << "< " << type << "> " << name << "_container_type;";
        }
        else
        {
          os << "typedef ACE_Refcounted_Auto_Ptr < " << type << ", ACE_Null_Mutex> " << name << "_value_type;";
          os << "typedef " << container  << "<" << name << "_value_type> " << name << "_container_type;";
        }
        if (!this->cpp11_)
        {
          os << "typedef " << name << "_container_type::iterator " << name << "_iterator;";
        }
        os << "typedef " << name << "_container_type::const_iterator " << name << "_const_iterator;";

        if (!this->cpp11_)
        {
          os << name << "_iterator begin_" << name << " ();";
          os << name << "_iterator end_" << name << " ();";
        }
        os << name << "_const_iterator begin_" << name << " () const;";
        os << name << "_const_iterator end_" << name << " () const;";

        if (!this->cpp11_)
        {
          os << "void add_" << name << " (" << name << "_value_type const&);";
        }
        //Return referenced item if an IDREF
        if ((idref_ptr != std::string::npos) && (!this->cpp11_))
        {
          os << "XSCRT::Type* get_" << name << "_ptr (const " << string_type <<"& idref);";
          os << "void set_" << name << "_ptr (const " << string_type << "& idref);";
        }
        //if (!this->cpp11_)
        {
          os << "size_t count_" << name << " () const;";
        }

        os << endl
           << "protected:" << endl;

        os << name << "_container_type " << id(name) << "_;";
      }
      else if (c.min () == 0)
      {
        // optional
        //
        os << "bool " << name << "_p () const;";
        os << type << " const& " << id (name) << " () const;";
        // os << type << "& " << id (name) << " ();";  // Lets just have one mutator
        os << "void " << id (name) << " (" << type << " const& );";

        //Added for IDREF case
        if ((idref_ptr != std::string::npos) && (!this->cpp11_))
        {
           os << "::XSCRT::Type* get_" << id (name) << "_ptr ();\n";
           os << "void set_" << id (name) << "_ptr (const " << string_type << "& idref);";
        }

        os << endl
           << "protected:" << endl;
        if (this->cpp11_)
        {
          os << "typedef std::unique_ptr< " << type << " > " << id (name) << "_unique_ptr_type;";
          os << id (name) << "_unique_ptr_type " << id (name) << "_;";
        }
        else
        {
          os << "typedef std::auto_ptr< " << type << " > " << id (name) << "_auto_ptr_type;";
          os << id (name) << "_auto_ptr_type " << id (name) << "_;";
        }
      }
      else
      {
        // one
        //
        os << type << " const& " << id (name) << " () const;";
        // os << type << "& " << id (name) << " ();"; // Lets just have one mutator.
        os << "void " << id (name) << " (" << type << " const& );";

        //Added for IDREF case
        if ((idref_ptr != std::string::npos) && (!this->cpp11_))
        {
           os << "::XSCRT::Type* get_" << id (name) << "_ptr ( void );\n";
           os << "void set_" << id(name) << "_ptr (const " << string_type << "& idref);";
        }

        os << endl
           << "protected:" << endl;

        if (this->cpp11_)
        {
          os << "typedef std::unique_ptr< " << type << " > " << id (name) << "_unique_ptr_type;";
          os << id (name) << "_unique_ptr_type " << id (name) << "_;";
        }
        else
        {
          os << "typedef std::auto_ptr< " << type << " > " << id (name) << "_auto_ptr_type;";
          os << id (name) << "_auto_ptr_type " << id (name) << "_;";
        }
      }

      os << endl;
    }
  };

  struct Attribute : Traversal::Attribute, protected virtual Context
  {
    Attribute (Context& c)
        : Context (c)
    {
    }

    string
    type_name (Type& i)
    {
      SemanticGraph::Type& t (i.type ());

      if (t.named ())
      {
        return Context::type_name (t);
      }
      else
      {
        return anon_prefix_ + id (i.name ()) + anon_suffix_;
      }
    }

    virtual void
    traverse (Type& a)
    {
      string name (a.name ());
      string type (type_name (a));

      //Check if the type is an IDREF
      size_t idref_ptr = 0;
      std::wstring idref_str (L"::XMLSchema::IDREF<");
      idref_ptr = type.find(idref_str);

      os << "// " << name << endl
         << "public:" << endl;


      if (a.optional ())
      {
        os << "bool " << name << "_p () const;";
        os << type << " const& " << id (name) << " () const;";
        os << type << "& " << id (name) << " ();";
        os << "void " << id (name) << " (" << type << " const& );";

        //Added for IDREF case
        if ((idref_ptr != std::string::npos) && (!this->cpp11_))
        {
           os << "::XSCRT::Type* get_" << id (name) << "_ptr ();\n";
           os << "void set_" << id(name) << "_ptr (const " << string_type << "& idref);";
        }

        os << endl
           << "protected:" << endl;

        if (this->cpp11_)
        {
          os << "typedef std::unique_ptr< " << type << " > " << id (name) << "_unique_ptr_type;";
          os << id (name) << "_unique_ptr_type " << id (name) << "_;";
        }
        else
        {
          os << "typedef std::auto_ptr< " << type << " > " << id (name) << "_auto_ptr_type;";
          os << id (name) << "_auto_ptr_type " << id (name) << "_;";
        }
      }
      else
      {
        os << type << " const& " << id (name) << " () const;";
        os << type << "& " << id (name) << " ();";
        os << "void " << id (name) << " (" << type << " const& );";

        //Added for IDREF case
        if ((idref_ptr != std::string::npos) && (!this->cpp11_))
        {
           os << "::XSCRT::Type* get_" << id (name) << "_ptr ( void );\n";
           os << "void set_" << id (name) << "_ptr (const " << string_type << "& idref);";
        }

        os << endl
           << "protected:" << endl;

        if (this->cpp11_)
        {
          os << "typedef std::unique_ptr< " << type << " > " << id (name) << "_unique_ptr_type;";
          os << id (name) << "_unique_ptr_type " << id (name) << "_;";
        }
        else
        {
          os << "typedef std::auto_ptr< " << type << " > " << id (name) << "_auto_ptr_type;";
          os << id (name) << "_auto_ptr_type " << id (name) << "_;";
        }
      }

      os << endl;
    }
  };


  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c,
             Traversal::NodeDispatcher& anonymous_type,
             string const& name_ = L"")
        : Context (c),
          name (name_),
          e (c.esymbol.empty () ? c.esymbol : c.esymbol + L" "),
          base_ (c),
          element_ (c),
          attribute_ (c),
          ctor_args_ (c)
    {
      edge_traverser (inherits_);
      edge_traverser (names_);

      inherits_.node_traverser (base_);
      names_.node_traverser (anonymous_type);
      names_.node_traverser (element_);
      names_.node_traverser (attribute_);
    }

    virtual void
    traverse (Type& c)
    {
      // We need to get the name of the element. The name would
      // be already set if it was an anonymous type.
      if (c.named ())
        name = id (c.name ());

      // We only continue if we have a valid name.
      if (!name.empty ())
      {
        enter_scope (name);

        Traversal::Complex::traverse (c);

        leave_scope ();
      }
    }

    virtual void
    pre (Type&)
    {
      os << "class " << e << name;
    }

    virtual void
    inherits_none (Type &c)
    {
      os << " : public ::XSCRT::Type"
         << "{"
        //         << "//@@ VC6 anathema" << endl
         << "typedef ::XSCRT::Type Base;"
         << endl;
      if (!this->cpp11_)
      {
        os << "public:" << endl;
        os << "typedef ACE_Refcounted_Auto_Ptr < " << type_name (c) << ", ACE_Null_Mutex> _ptr;";
        os << endl;
      }
    }

    virtual void
    inherits_post (Type &c)
    {
      if (!this->cpp11_)
      {
        os << "public:" << endl;
        os << "typedef ACE_Refcounted_Auto_Ptr < " << type_name(c) << ", ACE_Null_Mutex> _ptr;";
        os << endl;
      }
    }
    // Helper function to determine if read/write & >> and <<
    // operators need to be generated. Here are the cases where
    // the operators are generated:
    // 1: If the type is complex.
    // 2: If the type is simple and it has an attribute.

    bool
    generate_cdr_types (Type& c)
    {
      // Is type a complex type?
      Type::InheritsIterator b (c.inherits_begin ()), e (c.inherits_end ());
      if (b == e)
        return true;

      // Checks only for simple types having attributes.
      bool ret_val (has<Traversal::Attribute> (c));
      return ret_val;
    }

    virtual void
    read_write_type (Type &c)
    {
      if (! generate_cdr_types (c))
        return;

      // CDR Insertion/Extraction Operators
      bool reader = 0;

      if (this->cdr_reader_generation_)
      {
        os << "// read" << endl;

        reader = 1;

        os << "public:" << endl;
        os << "static bool" << endl
           << "read_" << name
           << " (::XMLSchema::CDR_InputStream &,"
           << endl;

        for (size_t j =0; j < name.length () + 5; j++)
          os << " ";

        os << " ::XMLSchema::cdr_arg_traits < " << name
           << " >::inout_type);" << endl;
      }

      // CDR Extraction Operators
      if (this->cdr_writer_generation_)
      {
        if (!reader)
          os << "public:" << endl;

        os << "// write" << endl
           << "bool" << endl
           << "write_" << name
           << " (::XMLSchema::CDR_OutputStream &) const;"
           << endl;
      }
    }

    virtual void
    post (Type& c)
    {
      os << "public:" << endl;

      // c-tor (all-non-optional-members)
      //
      os << name << " (";

      ctor_args_.traverse (c);

      os <<  ");"
         << endl;

      // c-tor (Element const&)
      //
      os << "explicit " << name << " (" << xml_element_type << " const&);";

      // c-tor (Attribute const&)
      //
      bool r (true);

      struct Traverser : Traversal::Inherits,
                         Traversal::Complex
      {
        Traverser (bool& v)
            : v_ (v)
        {
          edge_traverser (*this);
          node_traverser (*this);
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          if (has<Traversal::Element> (c) || has<Traversal::Attribute> (c))
            v_ = false;
          else
          {
            inherits (c, *this);
          }
        }

      private:
        bool& v_;
      } t (r);

      t.traverse (c);

      if (r)
      {
        os << "explicit " << name << " (" << xml_attribute_type << " const&);";
      }

      // copy c-tor
      //
      os << name << " (" << name << " const& s);";

      // operator=
      //
      os << name << "& operator= (" << name << " const& s);"
         << endl;

      if (this->cpp11_)
      {
        os << name << " (" << name << "&&) = default;";
        os << name << "& operator= (" << name << "&&) = default;"
           << endl;
      }

      // Closing brace for the class declaration
      // Example: class Foo {
      // };
      os << "};";
    }

    void
    insertion_extraction (Type &c)
    {
      // Check to see if >> and << operators need to
      // be generated
      if (! generate_cdr_types (c))
        return;

      if (this->cdr_reader_generation_)
        os << "extern bool" << endl
          << "operator >> (::XMLSchema::CDR_InputStream &,"
          << endl
          << "             ::XMLSchema::cdr_arg_traits < "
          << name << " >::inout_type);";

      if (this->cdr_writer_generation_)
        os << "extern bool" << endl
           << "operator << (::XMLSchema::CDR_OutputStream &,"
           << endl
           << "             ::XMLSchema::cdr_arg_traits < "
           << name << " >::in_type);"
           << endl;
      else
        os << endl;
    }

  private:
    string name;
    string e;

    Traversal::Inherits inherits_;
    Traversal::Names names_;

    Base base_;
    Element element_;
    Attribute attribute_;

    CtorArgs ctor_args_;
  };

  struct Enumerator : Traversal::Enumerator, protected virtual Context
  {
    explicit Enumerator (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      os << "static " << scope << " const " << id (e.name ()) << ";";
    }
  };

  struct Label : Traversal::Enumerator, protected virtual Context
  {
    Label (Context& c)
      : Context (c)
      , first_ (true)
    {
    }

    virtual void
    traverse (Type& e)
    {
      if (!first_)
        {
          os << ", ";
        }
      else first_ = false;

      os << id (e.name ()) << "_l";
    }

    void reset (void)
    {
      first_ = true;
    }

  private:
    /// Controls how commas are printed in the enumeration list.
    bool first_;
  };

  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c, string const& name_ = L"")
        : Context (c),
          name (name_),
          ex (c.esymbol.empty () ? c.esymbol : c.esymbol + L" "),
          enumerator_ (c),
          label_ (c)
    {
      names_enumerators_.node_traverser (enumerator_);
      names_labels_.node_traverser (label_);
    }

    virtual void
    traverse (Type& e)
    {
      if (e.named ())
        name = id (e.name ());

      os << "class " << ex << name << " : public ::XSCRT::Type"
         << "{"
         << "public:" << endl;

      enter_scope (name);

      // c-tor (Element const&)
      //
      os << "explicit " << name << " (" << xml_element_type << " const&);";

      // c-tor (Attribute const&)
      //
      os << "explicit " << name << " (" << xml_attribute_type << " const&);";

      os << endl;

      names (e, names_enumerators_);

      os << endl;

      os << "enum Value"
         << "{";

      label_.reset ();
      names (e, names_labels_);

      os << endl << "};"
         << endl;

      os << "Value" << endl
         << "integral () const;"
         << endl;

      os << "friend bool " << ex << endl
         << "operator== (" << name << " const& a, " << name << " const& b);"
         << endl;

      os << "friend bool " << ex << endl
         << "operator!= (" << name << " const& a, " << name << " const& b);"
         << endl;


      os << "private:" << endl;

      os << name << " (Value v);"
         << endl;

      os << "Value v_;";

      // CDR Insertion/Extraction Operators
      bool reader = 0;
      if (this->cdr_reader_generation_)
      {
        os << "// read" << endl;
        reader = 1;

        os << "public:" << endl;
        os << "static bool" << endl
           << "read_" << name
           << " (::XMLSchema::CDR_InputStream &,"
           << endl;

        for (size_t j =0; j < name.length () + 5; j++)
          os << " ";

        os << " ::XMLSchema::cdr_arg_traits < " << name
           << " >::inout_type);" << endl;
      }

      // CDR Extraction Operators
      if (this->cdr_writer_generation_enabled ())
        {
          if (!reader)
            os << "public:" << endl;

          os << "// write" << endl
             << "bool" << endl
             << "write_" << name
             << " (::XMLSchema::CDR_OutputStream &) const;"
             << endl;
        }

      leave_scope ();

      // End of class
      os << "};";

      os << "bool " << ex
         << "operator== (" << name << " const &a, " << name << " const &b);"
         << endl;

      os << "bool " << ex
         << "operator!= (" << name << " const &a, " << name << " const &b);"
         << endl;

      if (this->cdr_reader_generation_)
        os << "extern bool" << endl
           << "operator >> (::XMLSchema::CDR_InputStream &,"
           << endl
           << "             ::XMLSchema::cdr_arg_traits < "
           << name << " >::inout_type);";

      if (this->cdr_writer_generation_enabled ())
        os << "extern bool" << endl
           << "operator << (::XMLSchema::CDR_OutputStream &,"
           << endl
           << "             ::XMLSchema::cdr_arg_traits < "
           << name << " >::in_type);" << endl;
      else
        os << endl;
    }
  private:
    string name;
    string ex;

    Enumerator enumerator_;
    Traversal::Names names_enumerators_;

    Label label_;
    Traversal::Names names_labels_;
  };


  struct AnonymousType : Traversal::Element, protected virtual Context
  {
    explicit AnonymousType (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      SemanticGraph::Type& t (e.type ());

      if (!t.named () && !t.context ().count ("seen"))
      {
        string name (type_name (e));

        os << "// anonymous type for " /* << scope << "::"*/ << name << endl;

        if (dynamic_cast<SemanticGraph::Type*> (&e.scope ()))
        {
          os << "public:" << endl;
        }

        Traversal::Belongs belongs;
        Complex complex (*this, *this, id (name));
        Enumeration enumeration (*this, id (name));

        belongs.node_traverser (complex);
        belongs.node_traverser (enumeration);

        t.context ().set ("seen", true);

        Element::belongs (e, belongs);

        t.context ().remove ("seen");
      }
    }
  };


  struct Includes : Traversal::Imports,
                    Traversal::Includes,
                    protected virtual Context
  {
    Includes (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (SemanticGraph::Imports& i)
    {
      traverse (i.file ());
    }

    virtual void
    traverse (SemanticGraph::Includes& i)
    {
      traverse (i.file ());
    }

    virtual void
    traverse (fs::path const& f)
    {
      std::string name (regex::perl_s (f.string (), hxx_expr.c_str ()));
      name += hxx_suffix;

      os << "#include \"" << name.c_str () << "\"" << endl
         << endl;
    }

  private:
    std::string expr_;
  };
}

void
generate_header (Context& ctx,
                 SemanticGraph::Schema& schema)
{
  ctx.os << "#include <memory>" << endl;
  ctx.os << "#include <string>" << endl;

  if (ctx.generate_ra_sequences ())
    ctx.os << "#include <vector>" << endl;
  else
    ctx.os << "#include <list>" << endl;

  //Added
  ctx.os << "#include \"ace/XML_Utils/XMLSchema/Types.hpp\"" << endl
         << "#include \"ace/XML_Utils/XMLSchema/id_map.hpp\"" << endl;

  if (!ctx.cpp11())
  {
    ctx.os << "#include \"ace/Refcounted_Auto_Ptr.h\"" << endl
          << "#include \"ace/Null_Mutex.h\"" << endl;
  }

  if (ctx.char_type == L"ACE_TCHAR")
  {
    ctx.os << "#include \"ace/ace_wchar.h\"" << endl << endl;
  }

  // -- Include CDR Type headers if cdr generation is
  // enabled
  if (ctx.cdr_reader_generation_enabled () ||
      ctx.cdr_writer_generation_enabled ())
  {
    ctx.os << "#include \"XMLSchema/CDR_Types.hpp\"" << endl
           << "#include \"XMLSchema/id_map.hpp\"" << endl
           << endl;
    if (!ctx.cpp11())
    {
      ctx.os << "#include \"ace/TSS_T.h\""<< endl;
    }
  }

  if (ctx.cpp11())
  {
    ctx.os << "#include \"tao/x11/stddef.h\"" << endl;
  }

  Traversal::Schema traverser;

  Traversal::Sources sources;
  Includes includes (ctx);
  Traversal::Names schema_names;

  Namespace ns (ctx);

  traverser.edge_traverser (sources);
  traverser.edge_traverser (includes);
  traverser.edge_traverser (schema_names);
  sources.node_traverser (traverser);
  schema_names.node_traverser (ns);

  Traversal::Names names;

  AnonymousType anonymous_type (ctx);
  Complex complex (ctx, anonymous_type);
  Enumeration enumeration (ctx);

  ns.edge_traverser (names);
  names.node_traverser (complex);
  names.node_traverser (enumeration);
  names.node_traverser (anonymous_type);

  traverser.traverse (schema);
}
