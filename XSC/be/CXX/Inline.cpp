// file      : CXX/Inline.cxx
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include "Inline.hpp"
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
  struct Member : Traversal::Element,
                  Traversal::Attribute,
                  protected virtual Context
  {
    Member (Context& c, bool i_)
        : Context (c), i (i_ ? L"inline\n" : L"")
    {
    }

    void
    traverse (SemanticGraph::Element& e) override
    {
      string name (e.name ());
      string type (type_name (e));

      //bool ra_sequence (this->generate_ra_sequences_);

      //Check if the type is an IDREF
      std::string::size_type idref_ptr = 0;
      std::wstring idref_str (L"::XMLSchema::IDREF<");
      idref_ptr = type.find(idref_str);

      if (!this->read_only_)
        os << "// " << scope << std::endl;

      if (e.min () == 0 && e.max () == 1)
      {
        // optional
        //
        if (this->cppmode_ != CPPMODE::CPP03 || this->read_only_)
        {
          string noexcept_suffix (this->cppmode_ == CPPMODE::CPP03 ? L"" : L" noexcept");
          os << i << type << " const* " << scope << "::" << id (name)
             << "_if_set () const" << noexcept_suffix
             << "{return " << id (name) << "_.get ();}";

          if (!this->read_only_)
            os << i << type << "* " << scope << "::" << id (name)
               << "_if_set ()" << noexcept_suffix
               << "{return " << id (name) << "_.get ();}";
        }
        else
        {
          os << i
             << "bool " << scope << "::" << name << "_p () const"
             << "{return " << id (name) << "_.get () != 0;}";

          os << i
             << type << " const& " << scope << "::" << id (name) << " () const"
             << "{return *" << id (name) << "_;}";
        }

        // Return referenced item if an IDREF
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
           os << i
              << "::XSCRT::Type* " << scope << "::get_"
              << id (name) << "_ptr ()\n"
              << "{"
              << string_type << " temp ("
              << id (name) << "().id());"
              << "return this->get_idref(temp);\n"
              << "}\n";

           os << i
              << "void " << scope << "::set_"
              << id (name) << "_ptr (const " << string_type << "& idref)\n"
              << "{"
              << "  ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());"
              << "  (*TSS_ID_Map)->resolve_single_idref(idref, this);"
              << "}\n";
        }
        /* Lets just have one accessor. WRO
        os << i
           << type << "& " << scope << "::" << id (name) << " ()"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";
        */
        if (!this->read_only_)
        {
        os << i
           << "void " << scope << "::" << id (name) << " (" << type << " const& e)"
           << "{";

        if (this->cppmode_ != CPPMODE::CPP03)
        {
          os << "if (" << id (name) << "_)";
        }
        else
        {
          os << "if (" << id (name) << "_.get ())";
        }

        os << "{"
           << "*" << id (name) << "_ = e;"
           << "}"
           << "else"
           << "{";

          switch(this->cppmode_)
          {
            case CPPMODE::CPP03:
              os << id (name) << "_ = " << scope << "::" << id(name) << "_type (new " << type << " (e));"
                << id (name) << "_->container (this);";
              break;
            case CPPMODE::CPP11:
            case CPPMODE::CPP17:
              os << id (name) << "_ = std::make_unique<" << type << "> (e);";
              break;
        }
        os << "}"
           << "}";
        }
      }
      else if (e.min () == 1 && e.max () == 1)
      {
        // one
        //
        os << i
           << type << " const& " << scope << "::" << id (name) << " () const"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
           os << i
              << "::XSCRT::Type* " << scope << "::get_"
              << id (name) << "_ptr ()\n"
              << "{"
              << string_type << " temp ("
              << id (name) << "().id());"
              << "return this->get_idref(temp);\n"
              << "}\n";

           os << i
              << "void " << scope << "::set_"
              << id (name) << "_ptr (const " << string_type << "& idref)\n"
              << "{"
              << "  ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());"
              << "  (*TSS_ID_Map)->resolve_single_idref(idref, this);"
              << "}\n";
        }

        if (!this->read_only_)
          os << i
             << "void " << scope << "::" << id (name) << " (" << type << " const& e)"
             << "{"
             << "*" << id (name) << "_ = e;"
             << "}";
      }
      else
      {
        // sequence
        //

        if (!this->read_only_)
        {
          // begin_typename
          //
          os << i
            << scope << "::" << name << "_iterator " << scope << "::begin_" << name << " ()"
            << "{"
            << "return " << id(name) << "_.begin ();"
            << "}";

          // end_typename
          //
          os << i
            << scope << "::" << name << "_iterator " << scope << "::end_" << name << " ()"
            << "{"
            << "return " << id(name) << "_.end ();"
            << "}";
        }

        // begin_typename const
        os << i
           << scope << "::" << name << "_const_iterator " << scope << "::begin_" << name << " () const"
           << "{";

        if (this->cppmode_ != CPPMODE::CPP03)
        {
          os << "return " << id(name) << "_.cbegin ();";
        }
        else
        {
          os << "return " << id(name) << "_.begin ();";
        }
        os << "}";

        // end_typename const
        os << i
           << scope << "::" << name << "_const_iterator " << scope << "::end_" << name << " () const"
           << "{";

        if (this->cppmode_ != CPPMODE::CPP03)
        {
           os << "return " << id(name) << "_.cend ();";
        }
        else
        {
           os << "return " << id(name) << "_.end ();";
        }
        os << "}";

        // add IDREF access method
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
           os << i
              << "XSCRT::Type* " << scope << "::get_" << name
              << "_ptr (const " << string_type<< "& idref )"
              << "{"
              << "  return this->get_idref(idref);"
              << "}";

           os << i
              << "void " << scope << "::set_"
              << id (name) << "_ptr (const " << string_type << "& idref)\n"
              << "{"
              << "  ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());"
              << "  (*TSS_ID_Map)->resolve_single_idref(idref, this);"
              << "}\n";
        }

        if (this->cppmode_ == CPPMODE::CPP03 && !this->read_only_)
        {
          // add_typename
          os << i
            << "void " << scope << "::add_" << name << " (" << scope << "::" << name << "_value_type const& e)"
            << "{";
          os << id(name) << "_.push_back (e);";
          os << "}";
          // del_typename
          os << i
            << "void " << scope << "::del_" << name << " (" << scope << "::" << name << "_value_type const& e)"
            << "{";
          os << id(name) << "_.remove (e);";
          os << "}";
        }

        {
          // count_typename
          //
          os << i
            << "size_t " << scope << "::count_" << name << "() const"
            << "{"
            << "return " << id(name) << "_.size ();"
            << "}";
        }
      }
    }

    void
    traverse (SemanticGraph::Attribute& a) override
    {
      string name (a.name ());
      string type (type_name (a));

      if (!this->read_only_)
        os << "// " << scope << std::endl;

      //Check if the type is an IDREF
      std::string::size_type idref_ptr = 0;
      std::wstring idref_str (L"::XMLSchema::IDREF<");
      idref_ptr = type.find(idref_str);

      if (a.optional ())
      {
        if (this->cppmode_ != CPPMODE::CPP03 || this->read_only_)
        {
          string noexcept_suffix (this->cppmode_ == CPPMODE::CPP03 ? L"" : L" noexcept");
          os << i << type << " const* " << scope << "::" << id (name)
             << "_if_set () const" << noexcept_suffix
             << "{return " << id (name) << "_.get ();}";

          if (!this->read_only_)
            os << i << type << "* " << scope << "::" << id (name)
               << "_if_set ()" << noexcept_suffix
               << "{return " << id (name) << "_.get ();}";
        }
        else
        {
          os << i
            << "bool " << scope << "::" << name << "_p () const"
            << "{"
            << "return " << id (name) << "_.get () != 0;"
            << "}";
        }

        if (this->cppmode_ == CPPMODE::CPP03 && !this->read_only_)
        {
          os << i
             << type << " const& " << scope << "::" << id (name) << " () const"
             << "{return *" << id (name) << "_;}";

          os << i
             << type << "& " << scope << "::" << id (name) << " ()"
             << "{return *" << id (name) << "_;}";
        }

        // Return a pointer to the referenced item
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
           os << i
              << "::XSCRT::Type* " << scope << "::get_"
              << id (name) << "_ptr ()\n"
              << "{"
              << string_type << " temp ("
              << id (name) << "().id());"
              << "return this->get_idref(temp);\n"
              << "}\n";

           os << i
              << "void " << scope << "::set_"
              << id (name) << "_ptr (const " << string_type << "& idref)\n"
              << "{"
              << "  ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());"
              << "  (*TSS_ID_Map)->resolve_single_idref(idref, this);"
              << "}\n";
        }

        if (!this->read_only_)
        {
        os << i
           << "void " << scope << "::" << id (name) << " (" << type << " const& e)"
           << "{";

        if (this->cppmode_ != CPPMODE::CPP03)
        {
          os << "if (" << id (name) << "_)";
        }
        else
        {
          os << "if (" << id (name) << "_.get ())";
        }

        os << "{"
           << "*" << id (name) << "_ = e;"
           << "}"
           << "else"
           << "{";

        if (this->cppmode_ != CPPMODE::CPP03)
        {
          os << id (name) << "_ = std::make_unique<" << type << "> (e);";
        }
        else
        {
          os << id (name) << "_ = " << scope << "::" << id(name) << "_type (new " << type << " (e));"
             << id (name) << "_->container (this);";
        }
        os << "}"
           << "}";
        }
      }
      else
      {
        os << i
           << type << " const& " << scope << "::" << id (name) << " () const"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";

        if (!this->read_only_)
          os << i
             << type << "& " << scope << "::" << id (name) << " ()"
             << "{"
             << "return *" << id (name) << "_;"
             << "}";

        // Return a pointer to the referenced item
        if ((idref_ptr != std::string::npos) && (this->cppmode_ == CPPMODE::CPP03) &&
            !this->read_only_)
        {
           os << i
              << "::XSCRT::Type* " << scope << "::get_"
              << id (name) << "_ptr ()\n"
              << "{"
              << string_type << " temp ("
              << id (name) << "().id());"
              << "return this->get_idref(temp);\n"
              << "}\n";

           os << i
              << "void " << scope << "::set_"
              << id (name) << "_ptr (const " << string_type << "& idref)\n"
              << "{"
              << "  ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());"
              << "  (*TSS_ID_Map)->resolve_single_idref(idref, this);"
              << "}\n";
        }

        if (!this->read_only_)
          os << i
             << "void " << scope << "::" << id (name) << " (" << type << " const& e)"
             << "{"
             << "*" << id (name) << "_ = e;"
             << "}";
      }
    }

  private:
    string i;
  };

  // List of all non-optional members
  // @@ this code is the same as in Header.cpp
  //
  struct CtorArgs : Traversal::Complex,
                    protected Traversal::Enumeration,
                    //protected Traversal::Type,
                    protected Traversal::Element,
                    protected Traversal::Attribute,
                    protected virtual Context
  {
    explicit CtorArgs (Context& c)
        : Context (c)
    {
      complex_.edge_traverser (inherits_);
      complex_.edge_traverser (names_);

      inherits_.node_traverser (*this);
      names_.node_traverser (*this);
    }

    void
    traverse (SemanticGraph::Enumeration& e) override
    {
      os << comma () << type_name (e) << " const& b__";
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
           << id(name) << "__";
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

  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c,
             Traversal::NodeDispatcher& anonymous_type,
             bool i_,
             string const& name_ = L"")
        : Context (c),
          name (name_),
          i (i_ ? L"inline\n" : L""),
          ctor_base__ (c),
          ctor_member__ (c),
          ctor_body__ (c),
          copy_ (c),
          copy_body_ (c),
          assign_ (c),
          member_ (c, i_),
          ctor_args_ (c)
    {
      ctor_base_.node_traverser (ctor_base__);
      ctor_member_.node_traverser (ctor_member__);
      ctor_body_.node_traverser (ctor_body__);

      copy_base_.node_traverser (copy_);
      copy_member_.node_traverser (copy_);
      copy_body_member_.node_traverser (copy_body_);

      assign_base_.node_traverser (assign_);
      assign_member_.node_traverser (assign_);

      edge_traverser (names_);
      names_.node_traverser (anonymous_type);
      names_.node_traverser (member_);
    }

    void
    traverse (Type& c) override
    {
      if (c.named ())
        name = id (c.name ());

      if (!name.empty ())
      {
        enter_scope (name);

        c.context ().set ("name", scope);

        Traversal::Complex::traverse (c);

        c.context ().remove ("name");

        leave_scope ();
      }
    }


    void
    pre (Type& c) override
    {
      string type (name);

      if (!this->read_only_)
        os << "// " << scope << std::endl
           << std::endl;

      // c-tor
      //
      os << i
         << scope << "::" << name << " (";

      ctor_args_.traverse (c);

      os << ")" << std::endl;

      bool const has_base (c.inherits_begin () != c.inherits_end ());

      inherits (c, ctor_base_);

      // Resets a flag that determines if a Complex Type's constructor
      // has inherited from the base XSCRT::Type class. Right now, this
      // is commented out since it was producing a bug when complex types
      // extended another complete type.

      ctor_member__.reset_base_class_initialization (has_base);
      names (c, ctor_member_);

      os << "{";

      names (c, ctor_body_);

      os << "}";

      if (!this->read_only_)
      {
        // copy c-tor
        //
        os << i
           << scope << "::"
           << name << " (" << type << " const& s)" << std::endl;

        if (!has_base)
        {
          os << ": ::XSCRT::Type (s)" << std::endl;
        }

        // Resets a flag that determines if a Complex Type's
        // copy constructor has inherited from the base XSCRT::Type
        // class.
        copy_.reset_base_class_initialization ();

        inherits (c, copy_base_);

        names (c, copy_member_);

        os << "{";

        names (c, copy_body_member_);

        os << "}";

        std::wstring selfcheck;
        if (this->cppmode_ != CPPMODE::CPP03)
        {
          selfcheck = L"if (std::addressof(s) != this)";
        }
        else
        {
          selfcheck = L"if (&s != this)";
        }

        // operator=
        //
        os << i
           << scope << "& " << scope << "::operator= (" << type << " const& s)"
           << "{"
           << selfcheck
           << "{";

        inherits (c, assign_base_);
        names (c, assign_member_);

        os << "}"
           << "return *this;"
           << "}";

        os << std::endl;
      }
    }

    void
    post (Type&) override
    {
      os << std::endl;
    }

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
    insertion_extraction (Type &c) override
    {
      // Check if we need to generate
      // cdr insertion and extraction
      // operators
      if (! generate_cdr_types (c))
        return;

      string type (type_name (c));

      if (this->cdr_reader_generation_)
        os << "bool operator >> (::XMLSchema::CDR_InputStream &stream, ::XMLSchema::cdr_arg_traits < "
           << type << " >::inout_type element)"
           << std::endl
           << "{"
           << "return " << type << "::read_"
           << name << " (stream, element);"
           << std::endl
           << "}";

      if (this->cdr_writer_generation_)
        os << "bool operator << (::XMLSchema::CDR_OutputStream &stream, ::XMLSchema::cdr_arg_traits < "
           << type << " >::in_type element)"
           << std::endl
           << "{"
           << "return element.write_"
           << name << " (stream);"
           << std::endl
           << "}";
    }

    struct CtorBase : Traversal::Complex,
                      Traversal::Enumeration,
                      Traversal::Type,
                      protected Traversal::Element,
                      protected Traversal::Attribute,
                      protected virtual Context
    {
      CtorBase (Context& c)
          : Context (c), args_ (c)
      {
      }

      void
      traverse (SemanticGraph::Enumeration&) override
      {
        os << ": Base (b__)" << std::endl;
      }

      void
      traverse (SemanticGraph::Type&) override
      {
        os << ": Base ()" << std::endl;
      }

      void
      traverse (SemanticGraph::Complex& c) override
      {
        string type (type_name (c));
        string name (id (c.name ()));

        os << ": " << name << " (";

        args_.traverse (c);

        os << ")" << std::endl;
      }

    private:
      struct Args : Traversal::Complex,
                    protected Traversal::Enumeration,
                    protected Traversal::Type,
                    protected Traversal::Element,
                    protected Traversal::Attribute,
                    protected virtual Context
      {
        Args (Context& c)
            : Context (c)
        {
          complex_.edge_traverser (inherits_);
          complex_.edge_traverser (names_);

          inherits_.node_traverser (*this);
          names_.node_traverser (*this);
        }

        void
        traverse (SemanticGraph::Type&) override
        {
          os << comma () << "b__";
        }

        void
        traverse (SemanticGraph::Enumeration&) override
        {
          os << comma () << "b__";
        }

        void
        traverse (SemanticGraph::Element& e) override
        {
          if (e.min () == 1 && e.max () == 1)
          {
            os << comma () << id (e.name ()) << "__";
          }
          else if (e.min () >= 1)
          {
            os << comma () << id (e.name ()) << "__";
          }
        }

        void
        traverse (SemanticGraph::Attribute& a) override
        {
          if (!a.optional ())
          {
            os << comma () << id (a.name ()) << "__";
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
      } args_;
    };


    struct CTorMember : Traversal::Element,
                        Traversal::Attribute,
                        protected virtual Context
    {
      CTorMember (Context& c)
      : Context (c),
        base_class_initialized_ (false)
      {
      }

      void
      traverse (SemanticGraph::Element& e) override
      {
        ensure_base_class_initialization ();

        if (e.min () == 1 && e.max () == 1)
        {
          // one
          //
          string name (id (e.name ()));
          string type (type_name (e));

          if (this->cppmode_ != CPPMODE::CPP03)
          {
            os << ", " << name << "_ (std::make_unique<" << type << "> (" << name << "__))" << std::endl;
          }
          else
          {
            os << ", " << name << "_ (new " << type << " (" << name << "__))" << std::endl;
          }
        }
        else if (e.min () >= 1)
        {
          // more than one
          string name (id (e.name ()));
          string type (type_name (e));

          os << ", " << name << "_ ("<< name << "__)" << std::endl;
        }
      }

      void reset_base_class_initialization (bool initialized)
      {
        base_class_initialized_ = initialized;
      }

      void
      traverse (SemanticGraph::Attribute& a) override
      {
        if (!a.optional ())
        {
          ensure_base_class_initialization ();

          string name (id (a.name ()));
          string type (type_name (a));

          if (this->cppmode_ != CPPMODE::CPP03)
          {
            os << ", " << name << "_ (std::make_unique<" << type << "> (" << name << "__))" << std::endl;
          }
          else
          {
            os << ", " << name << "_ (new " << type << " (" << name << "__))" << std::endl;
          }
        }
      }

    private:
      void ensure_base_class_initialization ()
      {
        if (!base_class_initialized_)
        {
          os << ": ::XSCRT::Type ()" << std::endl;
          base_class_initialized_ = true;
        }
      }

      bool base_class_initialized_;
    };


    struct CTorBody : Traversal::Element,
                      Traversal::Attribute,
                      protected virtual Context
    {
      CTorBody (Context& c)
          : Context (c)
      {
      }

      void
      traverse (SemanticGraph::Element& e) override
      {
        if (e.min () == 1 && e.max () == 1 && (this->cppmode_ == CPPMODE::CPP03))
        {
          // one
          //
          os << id (e.name ()) << "_->container (this);";
        }
      }

      void
      traverse (SemanticGraph::Attribute& a) override
      {
        if (!a.optional () && (this->cppmode_ == CPPMODE::CPP03))
        {
          os << id (a.name ()) << "_->container (this);";
        }
      }
    };


    struct Copy : Traversal::Type,
                  Traversal::Element,
                  Traversal::Attribute,
                  protected virtual Context

    {
      Copy (Context& c)
      : Context (c),
        base_class_initialized_ (false)
      {
      }

      void
      traverse (SemanticGraph::Type&) override
      {
        if (!this->base_class_initialized_)
          {
            os << ": Base (s)" << std::endl;
            this->base_class_initialized_ = true;
          }
      }

      void
      traverse (SemanticGraph::Element& e) override
      {
        string name (id (e.name ()));
        string type (type_name (e));
        std::wstring nullptr_string;
        if (this->cppmode_ != CPPMODE::CPP03)
        {
          nullptr_string = L"nullptr";
        }
        else
        {
          nullptr_string = L"0";
        }

        // Call the ::XSCRT::Type () base class
        // constructor. Fix for compile warnings.
        if (! base_class_initialized_)
          {
            base_class_initialized_ = true;
          }

        if (e.min () == 0 && e.max () == 1)
          {
            // optional
            //
            if (this->cppmode_ != CPPMODE::CPP03)
            {
              os << ", " << name << "_ ("
                << "s." << name << "_ ? "
                << "std::make_unique<" << type << "> (*s." << name << "_) : " << nullptr_string << ")" << std::endl;
            }
            else
            {
              os << ", " << name << "_ ("
                << "s." << name << "_.get () ? "
                << "new " << type << " (*s." << name << "_) : " << nullptr_string << ")" << std::endl;
            }
          }
        else if (e.min () == 1 && e.max () == 1)
          {
            // one
            //
            if (this->cppmode_ != CPPMODE::CPP03)
            {
              os << ", " << name << "_ (std::make_unique<" << type << "> (*s." << name << "_))" << std::endl;
            }
            else
            {
              os << ", " << name << "_ (new " << type << " (*s." << name << "_))" << std::endl;
            }
          }
        else
          {
            // sequence
            //

            os << ", " << name << "_ (s." << name << "_)" << std::endl;
          }
      }

      void
      traverse (SemanticGraph::Attribute& a) override
      {
        string name (id (a.name ()));
        string type (type_name (a));
        std::wstring nullptr_string;
        if (this->cppmode_ != CPPMODE::CPP03)
        {
          nullptr_string = L"nullptr";
        }
        else
        {
          nullptr_string = L"0";
        }

        if (a.optional ())
        {
          if (this->cppmode_ != CPPMODE::CPP03)
          {
            os << ", " << name << "_ ("
              << "s." << name << "_ ? "
              << "std::make_unique<" << type << "> (*s." << name << "_) : " << nullptr_string << ")" << std::endl;
          }
          else
          {
            os << ", " << name << "_ ("
              << "s." << name << "_.get () ? "
              << "new " << type << " (*s." << name << "_) : " << nullptr_string << ")" << std::endl;
          }
        }
        else
        {
          if (this->cppmode_ != CPPMODE::CPP03)
          {
            os << ", " << name << "_ (std::make_unique<" << type << "> (*s." << name << "_))" << std::endl;
          }
          else
          {
            os << ", " << name << "_ (new " << type << " (*s." << name << "_))" << std::endl;
          }
        }
      }

      void reset_base_class_initialization ()
      {
        base_class_initialized_ = false;
      }

    private:
      bool base_class_initialized_;
    };

    struct CopyBody : Traversal::Element,
                      Traversal::Attribute,
                      protected virtual Context
    {
      CopyBody (Context& c)
          : Context (c)
      {
      }

      void
      traverse (SemanticGraph::Element& e) override
      {
        string name (e.name ());

        if (e.min () == 0 && e.max () == 1)
        {
          name = id (name);

          // optional
          //
          if (this->cppmode_ == CPPMODE::CPP03)
          {
            os << "if (" << name << "_.get ()) "
               << name << "_->container (this);";
          }
        }
        else if (e.min () == 1 && e.max () == 1 && this->cppmode_ == CPPMODE::CPP03)
        {
          os << id (name) << "_->container (this);";
        }
      }

      void
      traverse (SemanticGraph::Attribute& a) override
      {
        string name (id (a.name ()));

        if (a.optional ())
        {
          // optional
          //
          if (this->cppmode_ == CPPMODE::CPP03)
          {
            os << "if (" << name << "_.get ()) "
               << name << "_->container (this);";
          }
        }
        else
        {
          if (this->cppmode_ == CPPMODE::CPP03)
          {
            os << name << "_->container (this);";
          }
        }
      }
    };

    struct Assignment : Traversal::Type,
                        Traversal::Element,
                        Traversal::Attribute,
                        protected virtual Context
    {
      Assignment (Context& c)
          : Context (c)
      {
      }

      void
      traverse (SemanticGraph::Type&) override
      {
        os << "Base::operator= (s);" << std::endl;
      }

      void
      traverse (SemanticGraph::Element& e) override
      {
        string name (e.name ());
        string type (type_name (e));

        if (e.min () == 0 && e.max () == 1)
        {
          name = id (name);

          // optional
          //
          if (this->cppmode_ != CPPMODE::CPP03)
          {
            os << "if (s." << name << "_)" << std::endl
               << "  " << name << " (*(s." << name << "_));"
               << "else" << std::endl
               << "  " << name << "_.release ();"
               << std::endl;
          }
          else
          {
            os << "if (s." << name << "_.get ())" << std::endl
              << "  " << name << " (*(s." << name << "_));"
              << "else" << std::endl
              << "  " << name << "_.reset (0);"
              << std::endl;
          }
        }
        else if (e.min () == 1 && e.max () == 1)
        {
          name = id (name);

          // one
          //

          os << name << " (*s." << name << "_);"
             << std::endl;
        }
        else
        {
          // sequence
          //
          os << id(name) << "_ = s." << id(name) << "_;"
             << std::endl;
        }
      }

      void
      traverse (SemanticGraph::Attribute& a) override
      {
        string name (id (a.name ()));
        string type (type_name (a));

        if (a.optional ())
        {
          if (this->cppmode_ != CPPMODE::CPP03)
          {
            os << "if (s." << name << "_) "
              << name << " (*(s." << name << "_));"
              << "else " << name << "_.release ();"
              << std::endl;
          }
          else
          {
            os << "if (s." << name << "_.get ()) "
              << name << " (*(s." << name << "_));"
              << "else " << name << "_.reset (0);"
              << std::endl;
          }
        }
        else
        {
          os << name << " (s." << name << " ());"
             << std::endl;
        }
      }
    };

  private:
    string name;
    string i;

    CtorBase ctor_base__;
    CTorMember ctor_member__;
    CTorBody ctor_body__;
    Traversal::Inherits ctor_base_;
    Traversal::Names ctor_member_;
    Traversal::Names ctor_body_;

    Copy copy_;
    CopyBody copy_body_;
    Traversal::Inherits copy_base_;
    Traversal::Names copy_member_;
    Traversal::Names copy_body_member_;

    Assignment assign_;
    Traversal::Inherits assign_base_;
    Traversal::Names assign_member_;

    Traversal::Names names_;

    Member member_;

    CtorArgs ctor_args_;
  };


  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c, bool i_, string const& name_ = L"")
        : Context (c), name (name_), i (i_ ? L"inline\n" : L"")
    {
    }

    void
    traverse (Type& e) override
    {
      if (e.named ())
        name = id (e.name ());

      enter_scope (name);

      string type (type_name (e));

      if (!this->read_only_)
        os << "// " << scope << std::endl
           << std::endl;

      os << i
         << scope << "::Value " << scope << "::integral () const"
         << "{"
         << "return v_;"
         << "}";


      os << i
         << "bool operator== (" << type << " const& a, " << type << " const& b)"
         << "{"
         << "return a.v_ == b.v_;"
         << "}";

      os << i
         << "bool operator!= (" << type << " const& a, " << type << " const& b)"
         << "{"
         << "return a.v_ != b.v_;"
         << "}";

      os << i
         << scope << "::" << name << " (" << scope << "::Value v)" << std::endl
         << ": v_ (v)" << std::endl
         << "{"
         << "}";

      leave_scope ();

      // CDR Insertion extraction operations if needed
      if (this->cdr_reader_generation_)
        os << "bool operator >> (::XMLSchema::CDR_InputStream &stream, ::XMLSchema::cdr_arg_traits < "
          << type << " >::inout_type element)"
          << std::endl
          << "{"
          << "return " << type << "::read_"
          << name << " (stream, element);"
          << std::endl
          << "}";

      if (this->cdr_writer_generation_)
        os << "bool operator << (::XMLSchema::CDR_OutputStream &stream, ::XMLSchema::cdr_arg_traits < "
          << type << " >::in_type element)"
          << std::endl
          << "{"
          << "return element.write_"
          << name << " (stream);"
          << std::endl
          << "}";
    }

  private:
    string name;
    string i;
  };

  struct AnonymousType : Traversal::Element, protected virtual Context
  {
    AnonymousType (Context& c, bool i)
        : Context (c), i_ (i)
    {
    }

    void
    traverse (Type& e) override
    {
      SemanticGraph::Type& t (e.type ());

      if (!t.named () && !t.context ().count ("seen"))
      {
        string name (type_name (e));

        Traversal::Belongs belongs;
        Complex complex (*this, *this, i_, name);
        Enumeration enumeration (*this, i_, name);

        belongs.node_traverser (complex);
        belongs.node_traverser (enumeration);

        t.context ().set ("seen", true);

        Element::belongs (e, belongs);

        t.context ().remove ("seen");
      }
    }

  private:
    bool i_;
  };
}

void
generate_inline (Context& ctx, SemanticGraph::Schema& schema, bool i)
{
  if (ctx.char_type == L"ACE_TCHAR")
  {
    ctx.os << "#include \"ace/ace_wchar.h\"" << std::endl;
  }

  if (ctx.cppmode() == Context::CPPMODE::CPP03)
  {
    ctx.os << "#include \"ace/Null_Mutex.h\"" << std::endl
      << "#include \"ace/TSS_T.h\""<< std::endl
      << "#include \"ace/Singleton.h\"" << std::endl << std::endl;
  }

  Traversal::Schema traverser;
  Traversal::Sources sources;
  Traversal::Names schema_names;
  Namespace ns (ctx);

  traverser.edge_traverser (sources);
  traverser.edge_traverser (schema_names);
  sources.node_traverser (traverser);
  schema_names.node_traverser (ns);

  Traversal::Names names;
  AnonymousType anonymous_type (ctx, i);
  Complex complex (ctx, anonymous_type, i);
  Enumeration enumeration (ctx, i);

  ns.edge_traverser (names);

  names.node_traverser (complex);
  names.node_traverser (enumeration);
  names.node_traverser (anonymous_type);

  traverser.traverse (schema);
}
