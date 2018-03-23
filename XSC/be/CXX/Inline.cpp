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

    virtual void
    traverse (SemanticGraph::Element& e)
    {
      string name (e.name ());
      string type (type_name (e));
      string char_typ (this->char_type);

      //bool ra_sequence (this->generate_ra_sequences_);

      //Check if the type is an IDREF
      std::string::size_type idref_ptr = 0;
      std::wstring idref_str (L"::XMLSchema::IDREF<");
      idref_ptr = type.find(idref_str);

      os << "// " << scope << endl;

      if (e.min () == 0 && e.max () == 1)
      {
        // optional
        //
        os << i
           << "bool " << scope << "::" << endl
           << name << "_p () const"
           << "{";

        if (this->cpp11_)
        {
          os << "return !!" << id (name) << "_;";
        }
        else
        {
          os << "return " << id (name) << "_.get () != 0;";
        }

        os << "}";

        os << i
           << type << " const& " << scope << "::" << endl
           << id (name) << " () const"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";

        // Return referenced item if an IDREF
        if (idref_ptr != std::string::npos)
        {
           os << i
              << "::XSCRT::Type* " << scope << "::get_"
              << id (name) << "_ptr ()\n"
              << "{"
              << string_type << " temp ("
              << id (name) << "().id());"
              << "return this->get_idref(temp.c_str ());\n"
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
           << type << "& " << scope << "::" << endl
           << id (name) << " ()"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";
        */
        os << i
           << "void " << scope << "::" << endl
           << id (name) << " (" << type << " const& e)"
           << "{";

        if (this->cpp11_)
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

        if (this->cpp11_)
        {
          os << id (name) << "_ = std::make_unique< " << type << "> (e);";
        }
        else
        {
          os << id (name) << "_ = " << scope << "::" << id(name) << "_auto_ptr_type (new " << type << " (e));";
        }
        os << id (name) << "_->container (this);"
           << "}"
           << "}";
      }
      else if (e.min () == 1 && e.max () == 1)
      {
        // one
        //
        os << i
           << type << " const& " << scope << "::" << endl
           << id (name) << " () const"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";
        /* Lets just have one mutator
        os << i
           << type << "& " << scope << "::" << endl
           << id (name) << " ()"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";
        */
        if (idref_ptr != std::string::npos)
        {
           os << i
              << "::XSCRT::Type* " << scope << "::get_"
              << id (name) << "_ptr ()\n"
              << "{"
              << string_type << " temp ("
              << id (name) << "().id());"
              << "return this->get_idref(temp.c_str ());\n"
              << "}\n";

           os << i
              << "void " << scope << "::set_"
              << id (name) << "_ptr (const " << string_type << "& idref)\n"
              << "{"
              << "  ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());"
              << "  (*TSS_ID_Map)->resolve_single_idref(idref, this);"
              << "}\n";
        }

        os << i
           << "void " << scope << "::" << endl
           << id (name) << " (" << type << " const& e)"
           << "{"
           << "*" << id (name) << "_ = e;"
           << "}";
      }
      else
      {
        // sequence
        //

        // begin_typename
        //
        os << i
           << scope << "::" << name << "_iterator " << scope << "::" << endl
           << "begin_" << name << " ()"
           << "{"
           << "return " << id(name) << "_.begin ();"
           << "}";

        // end_typename
        //
        os << i
           << scope << "::" << name << "_iterator " << scope << "::" << endl
           << "end_" << name << " ()"
           << "{"
           << "return " << id(name) << "_.end ();"
           << "}";

        // begin_typename const
        os << i
           << scope << "::" << name << "_const_iterator " << scope << "::" << endl
           << "begin_" << name << " () const"
           << "{";

        if (this->cpp11_)
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
           << scope << "::" << name << "_const_iterator " << scope << "::" << endl
           << "end_" << name << " () const"
           << "{";

        if (this->cpp11_)
        {
           os << "return " << id(name) << "_.cend ();";
        }
        else
        {
           os << "return " << id(name) << "_.end ();";
        }
        os << "}";

        // add IDREF access method
        if (idref_ptr != std::string::npos)
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

        if (!this->cpp11_)
        {
          // add_typename
          os << i
            << "void " << scope << "::" << endl
            << "add_" << name << " (" << scope << "::" << name << "_value_type const& e)"
            << "{";

          os << id(name) << "_.push_back (e);";
          os << "}";
        }

//        if (!this->cpp11_)
        {
          // count_typename
          //
          os << i
            << "size_t " << scope << "::" << endl
            << "count_" << name << "(void) const"
            << "{"
            << "return " << id(name) << "_.size ();"
            << "}";
        }
      }
    }

    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      string name (a.name ());
      string type (type_name (a));

      os << "// " << scope << endl;

      //Check if the type is an IDREF
      std::string::size_type idref_ptr = 0;
      std::wstring idref_str (L"::XMLSchema::IDREF<");
      idref_ptr = type.find(idref_str);

      if (a.optional ())
      {
        if (this->cpp11_)
        {
          os << i
            << "bool " << scope << "::" << endl
            << name << "_p () const"
            << "{"
            << "return !!" << id (name) << "_;"
            << "}";
        }
        else
        {
          os << i
            << "bool " << scope << "::" << endl
            << name << "_p () const"
            << "{"
            << "return " << id (name) << "_.get () != 0;"
            << "}";
        }

        os << i
           << type << " const& " << scope << "::" << endl
           << id (name) << " () const"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";

        os << i
           << type << "& " << scope << "::" << endl
           << id (name) << " ()"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";

        // Return a pointer to the referenced item
        if (idref_ptr != std::string::npos)
        {
           os << i
              << "::XSCRT::Type* " << scope << "::get_"
              << id (name) << "_ptr ()\n"
              << "{"
              << string_type << " temp ("
              << id (name) << "().id());"
              << "return this->get_idref(temp.c_str ());\n"
              << "}\n";

           os << i
              << "void " << scope << "::set_"
              << id (name) << "_ptr (const " << string_type << "& idref)\n"
              << "{"
              << "  ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());"
              << "  (*TSS_ID_Map)->resolve_single_idref(idref, this);"
              << "}\n";
        }

        os << i
           << "void " << scope << "::" << endl
           << id (name) << " (" << type << " const& e)"
           << "{";

        if (this->cpp11_)
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

        if (this->cpp11_)
        {
          os << id (name) << "_ = std::make_unique< " << type << "> (e);";
        }
        else
        {
          os << id (name) << "_ = " << scope << "::" << id(name) << "_auto_ptr_type (new " << type << " (e));";
        }
        os << id (name) << "_->container (this);"
           << "}"
           << "}";
      }
      else
      {
        os << i
           << type << " const& " << scope << "::" << endl
           << id (name) << " () const"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";

        os << i
           << type << "& " << scope << "::" << endl
           << id (name) << " ()"
           << "{"
           << "return *" << id (name) << "_;"
           << "}";

        //Return a pointer to the referenced item
        if (idref_ptr != std::string::npos)
        {
           os << i
              << "::XSCRT::Type* " << scope << "::get_"
              << id (name) << "_ptr ()\n"
              << "{"
              << string_type << " temp ("
              << id (name) << "().id());"
              << "return this->get_idref(temp.c_str ());\n"
              << "}\n";

           os << i
              << "void " << scope << "::set_"
              << id (name) << "_ptr (const " << string_type << "& idref)\n"
              << "{"
              << "  ID_Map::TSS_ID_Map* TSS_ID_Map (ACE_Singleton<ID_Map::TSS_ID_Map, ACE_Null_Mutex>::instance());"
              << "  (*TSS_ID_Map)->resolve_single_idref(idref, this);"
              << "}\n";
        }

        os << i
           << "void " << scope << "::" << endl
           << id (name) << " (" << type << " const& e)"
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
    CtorArgs (Context& c)
        : Context (c)
    {
      complex_.edge_traverser (inherits_);
      complex_.edge_traverser (names_);

      inherits_.node_traverser (*this);
      names_.node_traverser (*this);
    }

    //virtual void
    //traverse (SemanticGraph::Type& t)
    //{
    //  os << comma () << type_name (t) << " const& b__";
    //}

    virtual void
    traverse (SemanticGraph::Enumeration& e)
    {
      os << comma () << type_name (e) << " const& b__";
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
           << id(name) << "__";
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

    virtual void
    traverse (Type& c)
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


    virtual void
    pre (Type& c)
    {
      string type (name);

      os << "// " << scope << endl
         << endl;

      // c-tor
      //
      os << i
         << scope << "::" << name << " (";

      ctor_args_.traverse (c);

      os << ") :" << endl;

      inherits (c, ctor_base_);

      // Resets a flag that determines if a Complex Type's constructor
      // has inherited from the base XSCRT::Type class. Right now, this
      // is commented out since it was producing a bug when complex types
      // extended another complete type.

      ctor_member__.reset_base_class_initialization ();
      names (c, ctor_member_);

      os << "regulator__ ()"
         << "{";

      names (c, ctor_body_);

      os << "}";

      // copy c-tor
      //
      os << i
         << scope << "::"
         << name << " (" << type << " const& s) :" << endl
         << "::XSCRT::Type (s)," << endl;

      // Resets a flag that determines if a Complex Type's
      // copy constructor has inherited from the base XSCRT::Type
      // class.
      copy_.reset_base_class_initialization ();

      inherits (c, copy_base_);

      names (c, copy_member_);

      os << "regulator__ ()"
         << "{";

      names (c, copy_body_member_);

      os << "}";

      std::wstring selfcheck;
      if (this->cpp11_)
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
         << scope << "&" << endl
         << scope << "::operator= (" << type << " const& s)"
         << "{"
         << selfcheck
         << "{";

      inherits (c, assign_base_);
      names (c, assign_member_);

      os << "}"
         << "return *this;"
         << "}";

      os << endl;
    }

    virtual void
    post (Type&)
    {
      os << endl;
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
    insertion_extraction (Type &c)
    {
      // Check if we need to generate
      // cdr insertion and extraction
      // operators
      if (! generate_cdr_types (c))
        return;

      string type (type_name (c));

      if (this->cdr_reader_generation_)
        os << "bool" << endl
           << "operator >> (::XMLSchema::CDR_InputStream &stream,"
           << endl
           << "             ::XMLSchema::cdr_arg_traits < "
           << type << " >::inout_type element)"
           << endl
           << "{"
           << "return " << type << "::read_"
           << name << " (stream, element);"
           << endl
           << "}";

      if (this->cdr_writer_generation_)
        os << "bool" << endl
           << "operator << (::XMLSchema::CDR_OutputStream &stream,"
           << endl
           << "             ::XMLSchema::cdr_arg_traits < "
           << type << " >::in_type element)"
           << endl
           << "{"
           << "return element.write_"
           << name << " (stream);"
           << endl
           << "}";
    }

    struct CtorBase : Traversal::Complex,
                      Traversal::Enumeration,
                      //Traversal::Type,
                      protected Traversal::Element,
                      protected Traversal::Attribute,
                      protected virtual Context
    {
      CtorBase (Context& c)
          : Context (c), args_ (c)
      {
      }

      //virtual void
      //traverse (SemanticGraph::Type&)
      //{
      //  os << "Base (b__)," << endl;
      //}

      virtual void
      traverse (SemanticGraph::Enumeration&)
      {
        os << "Base (b__)," << endl;
      }

      void
      traverse (SemanticGraph::Complex& c)
      {
        string type (type_name (c));
        string name (id (c.name ()));

        os << name << " (";

        args_.traverse (c);

        os << ")," << endl;
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

        virtual void
        traverse (SemanticGraph::Type&)
        {
          os << comma () << "b__";
        }

        virtual void
        traverse (SemanticGraph::Enumeration&)
        {
          os << comma () << "b__";
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (e.min () == 1 && e.max () == 1)
          {
            os << comma () << id (e.name ()) << "__";
          }
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          if (!a.optional ())
          {
            os << comma () << id (a.name ()) << "__";
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
      } args_;
    };


    struct CTorMember : Traversal::Element,
                        Traversal::Attribute,
                        protected virtual Context
    {
      CTorMember (Context& c)
      : Context (c),
        base_class_initialized_ (0)
      {
      }

      virtual void
      traverse (SemanticGraph::Element& e)
      {
        // Copy constructor for a complex type needs
        // to inherit from the ::XSCRT::Type base class
        if (! base_class_initialized_)
        {
          os << "::XSCRT::Type ()," << endl;
          base_class_initialized_ = 1;
        }

        if (e.min () == 1 && e.max () == 1)
        {
          // one
          //
          string name (id (e.name ()));
          string type (type_name (e));

          if (this->cpp11_)
          {
            os << name << "_ (std::make_unique< " << type << "> (" << name << "__))," << endl;
          }
          else
          {
            os << name << "_ (new " << type << " (" << name << "__))," << endl;
          }
        }
        else if (e.min () >= 1)
        {
          // more than one
          string name (id (e.name ()));
          string type (type_name (e));

          os << name << "_ ("<< name << "__)," << endl;
        }
      }

      void reset_base_class_initialization ()
      {
        base_class_initialized_ = 0;
      }

      virtual void
      traverse (SemanticGraph::Attribute& a)
      {
        if (!a.optional ())
        {
          string name (id (a.name ()));
          string type (type_name (a));

          if (this->cpp11_)
          {
            os << name << "_ (std::make_unique< " << type << "> (" << name << "__))," << endl;
          }
          else
          {
            os << name << "_ (new " << type << " (" << name << "__))," << endl;
          }
        }
      }

    private:
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

      virtual void
      traverse (SemanticGraph::Element& e)
      {
        if (e.min () == 1 && e.max () == 1)
        {
          // one
          //
          os << id (e.name ()) << "_->container (this);";
        }
      }

      virtual void
      traverse (SemanticGraph::Attribute& a)
      {
        if (!a.optional ())
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
        base_class_initialized_ (0)
      {
      }

      virtual void
      traverse (SemanticGraph::Type&)
      {
        if (!this->base_class_initialized_)
          {
            os << "Base (s)," << endl;
            this->base_class_initialized_ = 1;
          }
      }

      virtual void
      traverse (SemanticGraph::Element& e)
      {
        string name (id (e.name ()));
        string type (type_name (e));
        std::wstring nullptr_string;
        if (this->cpp11_)
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
            base_class_initialized_ = 1;
          }

        if (e.min () == 0 && e.max () == 1)
          {
            // optional
            //
            if (this->cpp11_)
            {
              os << name << "_ ("
                << "s." << name << "_ ? "
                << "std::make_unique< " << type << "> (*s." << name << "_) : " << nullptr_string << ")," << endl;
            }
            else
            {
              os << name << "_ ("
                << "s." << name << "_.get () ? "
                << "new " << type << " (*s." << name << "_) : " << nullptr_string << ")," << endl;
            }
          }
        else if (e.min () == 1 && e.max () == 1)
          {
            // one
            //
            if (this->cpp11_)
            {
              os << name << "_ (std::make_unique< " << type << "> (*s." << name << "_))," << endl;
            }
            else
            {
              os << name << "_ (new " << type << " (*s." << name << "_))," << endl;
            }
          }
        else
          {
            // sequence
            //

            os << name << "_ (s." << name << "_)," << endl;
          }
      }

      virtual void
      traverse (SemanticGraph::Attribute& a)
      {
        string name (id (a.name ()));
        string type (type_name (a));
        std::wstring nullptr_string;
        if (this->cpp11_)
        {
          nullptr_string = L"nullptr";
        }
        else
        {
          nullptr_string = L"0";
        }

        if (a.optional ())
        {
          os << name << "_ ("
             << "s." << name << "_.get () ? "
             << "new " << type << " (*s." << name << "_) : " << nullptr_string << ")," << endl;
        }
        else
        {
          if (this->cpp11_)
          {
            os << name << "_ (std::make_unique< " << type << "> (*s." << name << "_))," << endl;
          }
          else
          {
            os << name << "_ (new " << type << " (*s." << name << "_))," << endl;
          }
        }
      }

      void reset_base_class_initialization ()
      {
        base_class_initialized_ = 0;
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

      virtual void
      traverse (SemanticGraph::Element& e)
      {
        string name (e.name ());

        if (e.min () == 0 && e.max () == 1)
        {
          name = id (name);

          // optional
          //
          if (this->cpp11_)
          {
            os << "if (" << name << "_) "
              << name << "_->container (this);";
          }
          else
          {
            os << "if (" << name << "_.get ()) "
              << name << "_->container (this);";
          }
        }
        else if (e.min () == 1 && e.max () == 1)
        {
          os << id (name) << "_->container (this);";
        }
        else
        {
          //string type (type_name (e));

          //// sequence
          ////
          //if (this->generate_ra_sequences_)
          //  os << name << "_.reserve (s." << name << "_.size ());";

          //os << "{"
          //   << "for (" << name << "_const_iterator i (s."
          //   << name << "_.begin ());"
          //   << "i != s." << name << "_.end ();"
          //   << "++i) " << "add_" << name << " (*i);"
          //   << "}";
        }
      }

      virtual void
      traverse (SemanticGraph::Attribute& a)
      {
        string name (id (a.name ()));

        if (a.optional ())
        {
          // optional
          //
          if (this->cpp11_)
          {
            os << "if (" << name << "_) "
              << name << "_->container (this);";
          }
          else
          {
            os << "if (" << name << "_.get ()) "
              << name << "_->container (this);";
          }
        }
        else
        {
          os << name << "_->container (this);";
        }
      }
    };

    struct Assignment : /*Traversal::Type, */
                        Traversal::Element,
                        Traversal::Attribute,
                        protected virtual Context
    {
      Assignment (Context& c)
          : Context (c)
      {
      }

      //virtual void
      //traverse (SemanticGraph::Type&)
      //{
      //  os << "static_cast< Base& > (*this) = "
      //     << "static_cast< Base const& > (s);"
      //     << endl;
      //}

      virtual void
      traverse (SemanticGraph::Element& e)
      {
        string name (e.name ());
        string type (type_name (e));

        if (e.min () == 0 && e.max () == 1)
        {
          name = id (name);

          // optional
          //
          if (this->cpp11_)
          {
            os << "if (s." << name << "_)" << std::endl
               << "  " << name << " (*(s." << name << "_));"
               << "else" << std::endl
               << "  " << name << "_.reset (nullptr);"
               << endl;
          }
          else
          {
            os << "if (s." << name << "_.get ())" << std::endl
              << "  " << name << " (*(s." << name << "_));"
              << "else" << std::endl
              << "  " << name << "_.reset (0);"
              << endl;
          }
        }
        else if (e.min () == 1 && e.max () == 1)
        {
          name = id (name);

          // one
          //

          os << name << " (*s." << name << "_);"
             << endl;
        }
        else
        {
          // sequence
          //
          os << id(name) << "_ = s." << id(name) << "_;"
             << std::endl;

          //os << name << "_.clear ();";

          //if (this->generate_ra_sequences_)
          //  os   << name << "_.reserve (s." << name << "_.size ());";

          //os << "{"
          //   << "for (" << name << "_const_iterator i (s."
          //   << name << "_.begin ());"
          //   << "i != s." << name << "_.end ();"
          //   << "++i) " << std::endl
          //   << "add_" << name << " (*i);"
          //   << "}";
        }
      }

      virtual void
      traverse (SemanticGraph::Attribute& a)
      {
        string name (id (a.name ()));
        string type (type_name (a));

        if (a.optional ())
        {
          if (this->cpp11_)
          {
            os << "if (s." << name << "_) "
              << name << " (*(s." << name << "_));"
              << "else " << name << "_.reset (nullptr);"
              << endl;
          }
          else
          {
            os << "if (s." << name << "_.get ()) "
              << name << " (*(s." << name << "_));"
              << "else " << name << "_.reset (0);"
              << endl;
          }
        }
        else
        {
          os << name << " (s." << name << " ());"
             << endl;
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

    virtual void
    traverse (Type& e)
    {
      if (e.named ())
        name = id (e.name ());

      enter_scope (name);

      string type (type_name (e));

      os << "// " << scope << endl
         << endl;

      os << i
         << scope << "::Value " << scope << "::" <<  endl
         << "integral () const"
         << "{"
         << "return v_;"
         << "}";


      os << i
         << "bool" << endl
         << "operator== (" << type << " const& a, " << type << " const& b)"
         << "{"
         << "return a.v_ == b.v_;"
         << "}";

      os << i
         << "bool" << endl
         << "operator!= (" << type << " const& a, " << type << " const& b)"
         << "{"
         << "return a.v_ != b.v_;"
         << "}";

      os << i
         << scope << "::" <<  endl
         << name << " (" << scope << "::Value v)" << endl
         << ": v_ (v)" << endl
         << "{"
         << "}";

      leave_scope ();

      // CDR Insertion extraction operations if needed
      if (this->cdr_reader_generation_)
  os << "bool" << endl
     << "operator >> (::XMLSchema::CDR_InputStream &stream,"
     << endl
     << "             ::XMLSchema::cdr_arg_traits < "
     << type << " >::inout_type element)"
     << endl
     << "{"
     << "return " << type << "::read_"
     << name << " (stream, element);"
     << endl
     << "}";

      if (this->cdr_writer_generation_)
  os << "bool" << endl
     << "operator << (::XMLSchema::CDR_OutputStream &stream,"
     << endl
     << "             ::XMLSchema::cdr_arg_traits < "
     << type << " >::in_type element)"
     << endl
     << "{"
     << "return element.write_"
     << name << " (stream);"
     << endl
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

    virtual void
    traverse (Type& e)
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
    ctx.os << "#include \"ace/ace_wchar.h\"" << endl;
  }

  ctx.os << "#include \"ace/Null_Mutex.h\"" << endl
     << "#include \"ace/TSS_T.h\""<< endl
     << "#include \"ace/Singleton.h\"" << endl << endl;

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
