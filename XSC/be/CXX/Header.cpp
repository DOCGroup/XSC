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

    void
    traverse (SemanticGraph::Type& c) override
    {
      string name (type_name (c));

      os << " : public " << name
         << "{";
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

    void
    traverse (SemanticGraph::Enumeration& e) override
    {
      os << comma () << type_name (e) << " const& " << e.name () << "__";
    }

    void
    traverse (SemanticGraph::Element& e) override
    {
      string type (type_name (e));
      string name (e.name ());

      if (e.min () == 1 && e.max () == 1)
      {
        os << comma () << type << " const& " << id (name) << "__";
      }
      else if (e.min () >= 1)
      {
        os << comma ()
           << name << "_container_type const& "
           << id (name) << "__";
      }
    }

    void
    traverse (SemanticGraph::Attribute& a) override
    {
      if (!a.optional ())
      {
        os << comma () << type_name (a) << " const& " << id (a.name ()) << "__";
      }
    }

    void
    traverse (SemanticGraph::Complex& c) override
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


    void
    traverse (Type& c) override
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

      os << "public:" << std::endl;


      if (c.max () != 1)
      {
        // sequence
        //
        switch(this->cppmode_)
        {
          case CPPMODE::CPP03:
            os << "typedef ACE_Refcounted_Auto_Ptr < " << type << ", ACE_Null_Mutex> " << name << "_value_type;";
            os << "typedef " << container  << "<" << name << "_value_type> " << name << "_container_type;";
            break;
          case CPPMODE::CPP11:
          case CPPMODE::CPP17:
            os << "using " << name << "_container_type = " << container  << "<" << type << ">;";
            break;
        }
        switch(this->cppmode_)
        {
          case CPPMODE::CPP03:
            if (!this->read_only_)
              os << "typedef " << name << "_container_type::iterator " << name << "_iterator;";
            os << "typedef " << name << "_container_type::const_iterator " << name << "_const_iterator;";
            if (!this->read_only_)
            {
              os << name << "_iterator begin_" << name << " ();";
              os << name << "_iterator end_" << name << " ();";
            }
            break;
          case CPPMODE::CPP11:
          case CPPMODE::CPP17:
            if (!this->read_only_)
              os << "using " << name << "_iterator = " << name << "_container_type::iterator;";
            os << "using " << name << "_const_iterator = " << name << "_container_type::const_iterator;";
            if (!this->read_only_)
            {
              os << name << "_iterator begin_" << name << " ();";
              os << name << "_iterator end_" << name << " ();";
            }
            break;
        }

        os << name << "_const_iterator begin_" << name << " () const;";
        os << name << "_const_iterator end_" << name << " () const;";

        switch(this->cppmode_)
        {
          case CPPMODE::CPP03:
            if (!this->read_only_)
            {
              os << "void add_" << name << " (" << name << "_value_type const&);";
              os << "void del_" << name << " (" << name << "_value_type const&);";
            }
            break;
          case CPPMODE::CPP11:
          case CPPMODE::CPP17:
            break;
        }
        //Return referenced item if an IDREF
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
          os << "XSCRT::Type* get_" << name << "_ptr (const " << string_type <<"& idref);";
          os << "void set_" << name << "_ptr (const " << string_type << "& idref);";
        }

        os << "size_t count_" << name << " () const"
           << (this->cppmode_ == CPPMODE::CPP17 ? " noexcept;" : ";");

        os << std::endl
           << (this->read_only_ ? "private:" : "protected:") << std::endl;

        os << name << "_container_type " << id(name) << "_"
           << (this->read_only_ && this->cppmode_ != CPPMODE::CPP03 ? L" {};" : L";");
      }
      else if (c.min () == 0)
      {
        // optional
        //
        if (!this->optional_via_if_set ())
        {
          os << "bool " << name << "_p () const;";
          os << type << " const& " << id (name) << " () const;";
        }
        else
        {
          string noexcept_suffix (this->cppmode_ == CPPMODE::CPP03 ? L"" : L" noexcept");
          os << type << " const* " << id (name) << "_if_set () const"
             << noexcept_suffix << ";";
          if (!this->read_only_)
            os << type << "* " << id (name) << "_if_set ()"
               << noexcept_suffix << ";";
        }
        // os << type << "& " << id (name) << " ();";  // Lets just have one mutator
        if (this->read_only_)
          os << "private:" << std::endl;
        else
          os << "void " << id (name) << " (" << type << " const&);";

        //Added for IDREF case
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03))
        {
          if (!this->read_only_)
          {
            os << "::XSCRT::Type* get_" << id (name) << "_ptr ();\n";
            os << "void set_" << id (name) << "_ptr (const " << string_type << "& idref);";
          }
        }

        if (!this->read_only_)
          os << std::endl << "protected:" << std::endl;
        switch(this->cppmode_)
        {
          case CPPMODE::CPP03:
            os << "typedef XML_XSC_SMART_PTR( " << type << ") " << id (name) << "_type;";
            os << id (name) << "_type " << id (name) << "_;";
            break;
          case CPPMODE::CPP11:
          case CPPMODE::CPP17:
            os << "using " << id (name) << "_type = std::unique_ptr<" << type << ">;";
            os << id (name) << "_type " << id (name) << "_"
               << (this->read_only_ ? L" {};" : L";");
            break;
        }
      }
      else
      {
        // one
        //
        os << type << " const& " << id (name) << " () const;";
        // os << type << "& " << id (name) << " ();"; // Lets just have one mutator.
        if (this->read_only_)
          os << "private:" << std::endl;
        else
          os << "void " << id (name) << " (" << type << " const&);";

        //Added for IDREF case
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
           os << "::XSCRT::Type* get_" << id (name) << "_ptr ( void );\n";
           os << "void set_" << id(name) << "_ptr (const " << string_type << "& idref);";
        }

        if (!this->read_only_)
          os << std::endl << "protected:" << std::endl;

        switch(this->cppmode_)
        {
          case CPPMODE::CPP03:
            os << "typedef XML_XSC_SMART_PTR( " << type << ") " << id (name) << "_type;";
            os << id (name) << "_type " << id (name) << "_;";
            break;
          case CPPMODE::CPP11:
          case CPPMODE::CPP17:
            os << "using " << id (name) << "_type = std::unique_ptr<" << type << ">;";
            os << id (name) << "_type " << id (name) << "_"
               << (this->read_only_ ? L" {};" : L";");
            break;
        }
      }

      os << std::endl;
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

    void
    traverse (Type& a) override
    {
      string name (a.name ());
      string type (type_name (a));

      //Check if the type is an IDREF
      size_t idref_ptr = 0;
      std::wstring idref_str (L"::XMLSchema::IDREF<");
      idref_ptr = type.find(idref_str);

      os << "public:" << std::endl;


      if (a.optional ())
      {
        if (!this->optional_via_if_set ())
        {
          os << "bool " << name << "_p () const;";
          os << type << " const& " << id (name) << " () const;";
          os << type << "& " << id (name) << " ();";
        }
        else
        {
          string noexcept_suffix (this->cppmode_ == CPPMODE::CPP03 ? L"" : L" noexcept");
          os << type << " const* " << id (name) << "_if_set () const"
             << noexcept_suffix << ";";
          if (!this->read_only_)
            os << type << "* " << id (name) << "_if_set ()"
               << noexcept_suffix << ";";
        }
        if (this->read_only_)
          os << "private:" << std::endl;
        else
          os << "void " << id (name) << " (" << type << " const&);";

        //Added for IDREF case
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
           os << "::XSCRT::Type* get_" << id (name) << "_ptr ();\n";
           os << "void set_" << id(name) << "_ptr (const " << string_type << "& idref);";
        }

        if (!this->read_only_)
          os << std::endl << "protected:" << std::endl;

        switch(this->cppmode_)
        {
          case CPPMODE::CPP03:
            os << "typedef XML_XSC_SMART_PTR( " << type << ") " << id (name) << "_type;";
            os << id (name) << "_type " << id (name) << "_;";
            break;
          case CPPMODE::CPP11:
          case CPPMODE::CPP17:
            os << "using " << id (name) << "_type = std::unique_ptr<" << type << ">;";
            os << id (name) << "_type " << id (name) << "_"
               << (this->read_only_ ? L" {};" : L";");
            break;
        }
      }
      else
      {
        os << type << " const& " << id (name) << " () const;";
        if (!this->read_only_)
          os << type << "& " << id (name) << " ();";
        if (this->read_only_)
          os << "private:" << std::endl;
        else
          os << "void " << id (name) << " (" << type << " const&);";

        //Added for IDREF case
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
           os << "::XSCRT::Type* get_" << id (name) << "_ptr ( void );\n";
           os << "void set_" << id (name) << "_ptr (const " << string_type << "& idref);";
        }

        if (!this->read_only_)
          os << std::endl << "protected:" << std::endl;

        switch(this->cppmode_)
        {
          case CPPMODE::CPP03:
            os << "typedef XML_XSC_SMART_PTR( " << type << ") " << id (name) << "_type;";
            os << id (name) << "_type " << id (name) << "_;";
            break;
          case CPPMODE::CPP11:
          case CPPMODE::CPP17:
            os << "using " << id (name) << "_type = std::unique_ptr<" << type << ">;";
            os << id (name) << "_type " << id (name) << "_"
               << (this->read_only_ ? L" {};" : L";");
            break;
        }
      }

      os << std::endl;
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

    void
    traverse (Type& c) override
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

    void
    pre (Type&) override
    {
      os << "class " << e << name;
    }

    void
    inherits_none (Type &c) override
    {
      os << " : public ::XSCRT::Type"
         << "{" << std::endl;
      switch(this->cppmode_)
      {
        case CPPMODE::CPP03:
          os << "public:" << std::endl;
          os << "typedef ACE_Refcounted_Auto_Ptr < " << type_name (c) << ", ACE_Null_Mutex> _ptr;";
          break;
        case CPPMODE::CPP11:
        case CPPMODE::CPP17:
          break;
      }
    }

    void
    inherits_post (Type &c) override
    {
      if (this->cppmode_ == CPPMODE::CPP03)
      {
        os << "public:" << std::endl;
        os << "typedef ACE_Refcounted_Auto_Ptr < " << type_name(c) << ", ACE_Null_Mutex> _ptr;";
        os << std::endl;
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

    void
    read_write_type (Type &c) override
    {
      if (! generate_cdr_types (c))
        return;

      // CDR Insertion/Extraction Operators
      bool reader = 0;

      if (this->cdr_reader_generation_)
      {
        os << "// read" << std::endl;

        reader = 1;

        os << "public:" << std::endl;
        os << "static bool" << std::endl
           << "read_" << name
           << " (::XMLSchema::CDR_InputStream &,"
           << std::endl;

        for (size_t j =0; j < name.length () + 5; j++)
          os << " ";

        os << " ::XMLSchema::cdr_arg_traits < " << name << " >::inout_type);" << std::endl;
      }

      // CDR Extraction Operators
      if (this->cdr_writer_generation_)
      {
        if (!reader)
          os << "public:" << std::endl;

        os << "// write" << std::endl
           << "bool" << std::endl
           << "write_" << name
           << " (::XMLSchema::CDR_OutputStream &) const;"
           << std::endl;
      }
    }

    void
    post (Type& c) override
    {
      os << "public:" << std::endl;

      // c-tor (all-non-optional-members)
      //
      if (this->read_only_)
      {
        if (this->cppmode_ == CPPMODE::CPP03)
          os << "private:" << std::endl
             << name << " ();"
             << "public:" << std::endl;
        else
          os << name << " () = delete;";
      }

      if (!this->read_only_)
      {
        os << name << " (";

        ctor_args_.traverse (c);

        bool const default_constructor (this->cppmode_ != CPPMODE::CPP03 &&
                                        !has_constructor_required_members (c));
        os << ")" << (default_constructor ? " = default;" : ";")
           << std::endl;
      }

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

        void
        traverse (SemanticGraph::Complex& c) override
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
      if (this->read_only_)
      {
        if (this->cppmode_ == CPPMODE::CPP03)
          os << "private:" << std::endl
             << name << " (" << name << " const&);";
        else
        {
          os << name << " (" << name << " const&) = delete;";
          os << name << " (" << name << "&&) = delete;";
        }

        if (this->cppmode_ == CPPMODE::CPP03)
          os << "private:" << std::endl
             << name << "& operator= (" << name << " const&);";
        else
        {
          os << name << "& operator= (" << name << " const&) = delete;";
          os << name << "& operator= (" << name << "&&) = delete;";
        }
      }
      else
      {
        bool const default_copy (this->cppmode_ == CPPMODE::CPP17 &&
                                 !has_copy_required_members (c));

        os << name << " (" << name << " const& s)"
           << (default_copy ? " = default;" : ";");

        // operator=
        //
        os << name << "& operator= (" << name << " const& s)"
           << (default_copy ? " = default;" : ";");

        if (this->cppmode_ != CPPMODE::CPP03)
        {
          os << name << " (" << name << "&&) = default;";
          os << name << "& operator= (" << name << "&&) = default;";
        }
      }
      os << std::endl;

      // Closing brace for the class declaration
      // Example: class Foo {
      // };
      os << "};";
    }

    void
    insertion_extraction (Type &c) override
    {
      // Check to see if >> and << operators need to
      // be generated
      if (! generate_cdr_types (c))
        return;

      if (this->cdr_reader_generation_)
        os << "extern bool" << std::endl
          << "operator >> (::XMLSchema::CDR_InputStream &,"
          << std::endl
          << "             ::XMLSchema::cdr_arg_traits < "
          << name << " >::inout_type);";

      if (this->cdr_writer_generation_)
        os << "extern bool" << std::endl
           << "operator << (::XMLSchema::CDR_OutputStream &,"
           << std::endl
           << "             ::XMLSchema::cdr_arg_traits < "
           << name << " >::in_type);"
           << std::endl;
      else
        os << std::endl;
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

    void
    traverse (Type& e) override
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

    void
    traverse (Type& e) override
    {
      if (!first_)
        {
          os << ", ";
        }
      else first_ = false;

      os << id (e.name ()) << "_l";
    }

    void reset ()
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
          ex_ (c.esymbol),
          ex_sp_ (c.esymbol.empty () ? c.esymbol : c.esymbol + L" "),
          enumerator_ (c),
          label_ (c)
    {
      names_enumerators_.node_traverser (enumerator_);
      names_labels_.node_traverser (label_);
    }

    void
    traverse (Type& e) override
    {
      if (e.named ())
        name = id (e.name ());

      os << "class " << ex_sp_ << name << " : public ::XSCRT::Type"
         << "{" << "public:" << std::endl;

      enter_scope (name);

      if (this->read_only_)
      {
        if (this->cppmode_ == CPPMODE::CPP03)
          os << "private:" << std::endl
             << name << " ();"
             << "public:" << std::endl;
        else
          os << name << " () = delete;";
      }

      // c-tor (Element const&)
      //
      os << "explicit " << name << " (" << xml_element_type << " const&);";

      // c-tor (Attribute const&)
      //
      os << "explicit " << name << " (" << xml_attribute_type << " const&);";

      os << std::endl;

      names (e, names_enumerators_);

      os << std::endl;

      os << "enum Value"
         << "{";

      label_.reset ();
      names (e, names_labels_);

      os << L"};\nValue integral () const;\n";

      if (this->read_only_)
      {
        if (this->cppmode_ == CPPMODE::CPP03)
          os << "private:" << std::endl
             << name << " (" << name << " const&);"
             << name << "& operator= (" << name << " const&);";
        else
        {
          os << name << " (" << name << " const&) = delete;";
          os << name << " (" << name << "&&) = delete;";
          os << name << "& operator= (" << name << " const&) = delete;";
          os << name << "& operator= (" << name << "&&) = delete;";
        }
      }

      os << "friend bool " << ex_sp_
         << "operator== (" << name << " const& a, " << name << " const& b);"
         << L'\n'
         << "friend bool " << ex_sp_
         << "operator!= (" << name << " const& a, " << name << " const& b);"
         << L'\n';


      os << "private:" << std::endl;

      os << name << " (Value v);"
         << std::endl;

      os << "Value v_;";

      // CDR Insertion/Extraction Operators
      bool reader = 0;
      if (this->cdr_reader_generation_)
      {
        os << "// read" << std::endl;
        reader = 1;

        os << "public:" << std::endl;
        os << "static bool" << std::endl
           << "read_" << name
           << " (::XMLSchema::CDR_InputStream &,"
           << std::endl;

        for (size_t j =0; j < name.length () + 5; j++)
          os << " ";

        os << " ::XMLSchema::cdr_arg_traits < " << name
           << " >::inout_type);" << std::endl;
      }

      // CDR Extraction Operators
      if (this->cdr_writer_generation_enabled ())
        {
          if (!reader)
            os << "public:" << std::endl;

          os << "// write" << std::endl
             << "bool" << std::endl
             << "write_" << name
             << " (::XMLSchema::CDR_OutputStream &) const;"
             << std::endl;
        }

      leave_scope ();

      // End of class
      os << "};";

      os << "bool " << ex_sp_
         << "operator== (" << name << " const &a, " << name << " const &b);"
         << std::endl;

      os << "bool " << ex_sp_
         << "operator!= (" << name << " const &a, " << name << " const &b);"
         << std::endl;

      if (this->cdr_reader_generation_)
        os << "extern bool" << std::endl
           << "operator >> (::XMLSchema::CDR_InputStream &,"
           << std::endl
           << "             ::XMLSchema::cdr_arg_traits < "
           << name << " >::inout_type);";

      if (this->cdr_writer_generation_enabled ())
        os << "extern bool" << std::endl
           << "operator << (::XMLSchema::CDR_OutputStream &,"
           << std::endl
           << "             ::XMLSchema::cdr_arg_traits < "
           << name << " >::in_type);" << std::endl;
      else
        os << std::endl;
    }
  private:
    string name;
    string ex_;
    string ex_sp_;

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

    void
    traverse (Type& e) override
    {
      SemanticGraph::Type& t (e.type ());

      if (!t.named () && !t.context ().count ("seen"))
      {
        string name (type_name (e));

        os << "// anonymous type for " /* << scope << "::"*/ << name << std::endl;

        if (dynamic_cast<SemanticGraph::Type*> (&e.scope ()))
        {
          os << "public:" << std::endl;
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

    void
    traverse (SemanticGraph::Imports& i) override
    {
      traverse (i.file ());
    }

    void
    traverse (SemanticGraph::Includes& i) override
    {
      traverse (i.file ());
    }

    virtual void
    traverse (std::filesystem::path const& f)
    {
      std::string name (regex::perl_s (f.string (), hxx_expr.c_str ()));
      name += hxx_suffix;

      os << "#include \"" << name.c_str () << "\"" << std::endl
         << std::endl;
    }

  private:
    std::string expr_;
  };
}

void
generate_header (Context& ctx,
                 SemanticGraph::Schema& schema)
{
  ctx.os << "#include <memory>" << std::endl;
  ctx.os << "#include <string>" << std::endl;

  if (ctx.generate_ra_sequences ())
    ctx.os << "#include <vector>" << std::endl;
  else
    ctx.os << "#include <list>" << std::endl;

  //Added
  ctx.os << "#include \"ace/XML_Utils/XMLSchema/Types.hpp\"" << std::endl
         << "#include \"ace/XML_Utils/XMLSchema/id_map.hpp\"" << std::endl;

  if (ctx.cppmode() == Context::CPPMODE::CPP03)
  {
    ctx.os << "#include \"ace/Refcounted_Auto_Ptr.h\"" << std::endl
          << "#include \"ace/Null_Mutex.h\"" << std::endl;
  }

  if (ctx.char_type == L"ACE_TCHAR")
  {
    ctx.os << "#include \"ace/ace_wchar.h\"" << std::endl << std::endl;
  }

  // -- Include CDR Type headers if cdr generation is
  // enabled
  if (ctx.cdr_reader_generation_enabled () ||
      ctx.cdr_writer_generation_enabled ())
  {
    ctx.os << "#include \"XMLSchema/CDR_Types.hpp\"" << std::endl
           << "#include \"XMLSchema/id_map.hpp\"" << std::endl
           << std::endl;
    if (ctx.cppmode() == Context::CPPMODE::CPP03)
    {
      ctx.os << "#include \"ace/TSS_T.h\""<< std::endl;
    }
  }

  if (ctx.orb() == Context::ORB::TAOX11)
  {
    ctx.os << "#include \"tao/x11/base/stddef.h\"" << std::endl;
  }

  Traversal::Schema traverser;

  Traversal::Sources sources;
  Includes includes (ctx);
  Traversal::Names schema_names;

  if (ctx.cppmode() == Context::CPPMODE::CPP03)
  {
    ctx.os << "#if !defined(XML_XSC_SMART_PTR)" << std::endl
           << "# if defined(ACE_HAS_CPP11)" << std::endl
           << "#   define XML_XSC_SMART_PTR(X) std::unique_ptr<X>" << std::endl
           << "# else" << std::endl
           << "#   define XML_XSC_SMART_PTR(X) std::auto_ptr<X>" << std::endl
           << "# endif" << std::endl
           << "#endif" << std::endl << std::endl;
  }

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
