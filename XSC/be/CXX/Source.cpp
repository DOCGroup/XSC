// file      : CXX/Source.cxx
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include "Source.hpp"
#include "Elements.hpp"

#include "XSC/SemanticGraph.hpp"
#include "XSC/Traversal.hpp"

#include "CCF/CodeGenerationKit/Regex.hpp"

#include <ctype.h>

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
  struct Element : Traversal::Element, protected virtual Context
  {
    Element (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& c)
    {
      string name (c.name ());
      string type (type_name (c));

      //@@ can I get rid of the temporary?
      //

      //@@ need to use FQ-names.
      //
      os << "if (n == " << L << "\"" << name << "\")"
         << "{";

      if (c.max () != 1)
      {
        // sequence
        //
        os << type << "::_ptr t (new " << type << " (e));"
           << "add_" << name << " (t);";
      }
      else if (c.min () == 0)
      {
        // optional
        //
        os << type << " t (e);"
           << id (name) << " (t);";
      }
      else
      {
        // one
        //
        os << id (name) << "_ = ::std::auto_ptr< " << type << " > (" <<
          "new " << type << " (e));"
           << id (name) << "_->container (this);";
      }

      os <<"}"
         << "else ";
    }
  };

  struct Attribute : Traversal::Attribute, protected virtual Context
  {
    Attribute (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& a)
    {
      string name (a.name ());
      string type (type_name (a));

      //@@ need to use FQ-names.
      //
      os << "if (n == " << L << "\"" << name << "\")"
         << "{";

      if (a.optional ())
      {
        os << type << " t (a);"
           << id (name) << " (t);";
      }
      else
      {
        os << id (name) << "_ = ::std::auto_ptr< " << type << " > (" <<
          "new " << type << " (a));"
           << id (name) << "_->container (this);";
      }

      os << "}"
         << "else ";
    }
  };

  // Traverser for CDR write operations
  struct CDR_WriterTraverser : Traversal::Complex,
             protected Traversal::Type,
             protected Traversal::Element,
             protected Traversal::Attribute,
             protected virtual Context
  {

  public:
    CDR_WriterTraverser (Context& c)
      : Context (c)
    {
      // Set up traversal mechanism
      complex_.edge_traverser (inherits_);
      complex_.edge_traverser (names_);

      inherits_.node_traverser (*this);
      names_.node_traverser (*this);
    }

    bool
    type_is_boolean (string &type)
    {
      const wchar_t *bool_type = L"::XMLSchema::boolean";
      return (type.compare (bool_type) == 0);
    }

    virtual void
    traverse (SemanticGraph::Type& c)
    {
      // Writing types that have extension base
      // from basic type

      // One needs to check for boolean
      // types. If a type is boolean, we
      // to write it out as stream.write_boolean
      // rather than as << name. This is because
      // there will be an operator overloading
      // mismatch if this occurs.
      string type (type_name (c));

      if (type_is_boolean (type))
        os << "stream.write_boolean (*this);" << endl;
      else
        // Convert it to its base type. This will
        // have an operator << defined to
        // serialize this type
        os << "stream << static_cast < const Base& > (*this);" << endl;
    }

    // Helper method implementation
    void
    write_sequence (SemanticGraph::Element &e)
    {
      string name (e.name ());
      string type (type_name (e));

      // Write length first
      os << "// " << name << endl;
      os << "size_t " << name << "_size = "
         << "this->" << name << "_.size ();"
         << "stream << " << name << "_size;"
         << endl;

      // Write each member next
      os << "for (" << name << "_const_iterator "
         << name << "_iter = " << "this->begin_" << name << " ();"
         << "    " << name << "_iter != this->end_" << name
         << " ();"
         << "    ++" << name << "_iter)"
         << "{";

      // Check if the type being written is a boolean
      // type
      if (type_is_boolean (type))
        os << "stream.write_boolean (*" << name << ");";
      else
        os << "stream << (*" << name << "_iter);";

      os << "}";
    }

    virtual void
    traverse (SemanticGraph::Element& e)
    {
      string name (e.name ());
      string type (type_name (e));

      if (e.max () != 1)
      {
        // Write sequence types after we write the attributes
        if (this->has_attributes_)
          // Safe hack: Memory not managed by container.
          // released elsewhere. So this is safe
          this->complex_list_.push_back (&e);
        else
          this->write_sequence (e);
      }
      else if (e.min () == 0)
      {
        // optional
        //
        this->optional_element_list_.push_back (&e);
      }
      else
      {
        name = id (name);

        //
        // one
        os << "// " << name << endl;

        // check if the type is a boolean type
        if (type_is_boolean (type))
          os << "stream.write_boolean (this->" << name
             << " ());" << endl;
        else
          os << "stream << this->" << name << " ();" << endl;
      }
    }

    // Entry point from which all other types will be
    // visited and traversed
    void
    traverse (SemanticGraph::Complex &c)
    {
      complex_.traverse (c);

      // Write out sequence types if any after
      // wrting out the element
      finally ();
    }

    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      string name (id (a.name ()));
      string type (type_name (a));

      if (a.optional ())
      {
        name = id (name);

        // optional
        //
        os << " // " << name << endl;
        os << "bool " << name << "_p = this->" << name << "_p ();";
        os << "stream.write_boolean (" << name << "_p" << ");";

        os << "if (" << name << "_p)"
          << "{";

        // check if the type is a boolean type
        if (type_is_boolean (type))
          os << "stream.write_boolean (this->" << name
            << " ());" << endl;
        else
          os << "stream << this->" << name << " ();";

        os << "}" << endl;
            }
            else
      {
  //
  // one

  // check if the type is a boolean type
  if (type_is_boolean (type))
    os << "// " << name << endl
       << "stream.write_boolean (this->" << name
       << " ());" << endl;
  else
    os << "// " << name << endl
       << "stream << this->" << name << " ();" << endl;
      }
    }

    void
    has_attributes (bool settings)
    {
      this->has_attributes_ = settings;
    }

    void
    reset_type_attribute_settings ()
    {
      this->has_attributes_ = 0;
      this->complex_list_.clear ();
      this->optional_element_list_.clear ();
    }

    void
    finally ()
    {
      // Write the sequence elements
      if (! this->complex_list_.empty ())
        {
          for (size_t i =0; i < this->complex_list_.size (); i++)
            this->write_sequence (*this->complex_list_[i]);

          // Write Optional elements AFTER sequence elements
          if (! this->optional_element_list_.empty ())
            {
              for (size_t i=0; i < this->optional_element_list_.size (); i++)
                {
                  string name (this->optional_element_list_[i]->name ());
                  string type (type_name (*this->optional_element_list_[i]));

                  os << "// " << name << endl;

                  // First write out the boolean value
                  os << "stream.write_boolean (" << name << "_p ());";

                  os << "if (" << name << "_p ())"
                    << endl;

                  // check if the type is a boolean type
                  if (type_is_boolean (type))
                    os << "  stream.write_boolean (this->" << name
                      << " ());" << endl;
                  else
                    os << "  stream << this->" << name << " ();" << endl;
                }
            }
        }
    }

  private:
    Traversal::Complex complex_;
    Traversal::Inherits inherits_;
    Traversal::Names names_;
    bool has_attributes_;
    std::vector < SemanticGraph::Element* > complex_list_;
    std::vector < SemanticGraph::Element* > optional_element_list_;
  };

  // Traverser for CDR read operations
  struct CDR_ReaderTraverser : Traversal::Complex,
             protected Traversal::Type,
             protected Traversal::Element,
             protected Traversal::Attribute,
             protected virtual Context
  {

  public:
    CDR_ReaderTraverser (Context& c)
      : Context (c)
    {
      // Set up traversal mechanism
      complex_.edge_traverser (inherits_);
      complex_.edge_traverser (names_);

      inherits_.node_traverser (*this);
      names_.node_traverser (*this);
    }

    /*
     * Helper method to check if a type needs to be read
     * via the value semantics or using a pointer
     * semantics. This method only works for predefined types.
     * such as bool, string, name ID, IDREF etc.
     */
    bool
    read_type_as_pointer (string &type)
    {
      //@@ Use a static map tp load the names
      // and make the search faster in O(log N)
      // time.
      if (type.find (L"::XMLSchema::string") != string::npos ||
          type.find (L"::XMLSchema::ID") != string::npos     ||
          type.find (L"::XMLSchema::IDREF") != string::npos  ||
          type.find (L"::XMLSchema::token") != string::npos  ||
          type.find (L"::XMLSchema::NMTOKEN") != string::npos||
          type.find (L"::XMLSchema::Name") != string::npos   ||
          type.find (L"::XMLSchema::NCName") != string::npos)
      {
        return true;
      }
      else
      {
        return false;
      }
    }

    /*
     * Reading a pointer type.
     * A type which is read in via the pointer semantics. This
     * corresponds to strings, ID, IDREFs and types that have
     * attributes defined in them.
     *
     * A type:which is read as a pointer.
     * so to read this type one needs to do the following:
     * type::CDR_Type__ *foo;
     * stream >> foo;
     * type element (foo);
     */
    void
    read_pointer_type (string &element_name, string &type)
    {
      os << "stream >> " << element_name << "_tmp;"
   << "std::auto_ptr < " << type << "::CDR_Type__ > "
   << "auto_" << element_name << " (" << element_name << "_tmp);"
   << type << " " << element_name << " (" << element_name
   << "_tmp);"
   << endl;
    }

    /*
     * A complex type is a schema defined type that does not
     * inherit from a XML type.
     */
    void
    read_complex_type (string &element_name, string &type)
    {
      os << "stream >> " << element_name << ";"
   << "std::auto_ptr < " << type << " > "
   << "auto_" << element_name << " (" << element_name << ");"
   << endl;
    }

    /*
     * Reading a simple type
     * This simple type is read in as value and not via the
     * inout semantics defined. So we need not create an
     * auto_ptr for the temporary
     *
     * The only thing that we need to be careful about is that
     * booleans are read in as a member function rather than
     * using the >> operator.
     */
    void
    read_simple_type (string &element_name, string &type)
    {
      if (type == L"::XMLSchema::boolean")
  os << "stream.read_boolean (" << element_name
     << "_tmp);";
      else
  os << "stream >> " << element_name << "_tmp;";

      os << type << " " << element_name << " ("
   << element_name << "_tmp);"
   << endl;
    }

    void
    read_attribute (string &element_name, string &type)
    {
      // An attribute should be a simple type
      // However, if the type is either sting, ID or IDREF
      // we use the pointer semantics to read the type
      // else we use the value semantics.
      bool pointer_semantics = read_type_as_pointer (type);

      if (pointer_semantics)
  os << type << "::CDR_Type__ *" << element_name << "_tmp (0)"
     << ";" ;
      else
  os << type << "::CDR_Type__ " << element_name << "_tmp"
   << ";" ;

      if (pointer_semantics)
  read_pointer_type (element_name, type);
      else
  read_simple_type (element_name, type);

      this->type_name_list_.push_back (element_name);
    }

    void
    read_optional_attribute (string &element_name, string &type)
    {
      // Read the boolean value and then
      // read the attribute
      os << "bool " << element_name << "_p;"
   << "stream.read_boolean (" << element_name << "_p);";

      // For an optional attribute; declare it first
      // Again bear in mind that even for simple types
      // such as strings, ID, IDREFs we read them as
      // pointers aka inout semantics.
      bool pointer_semantics = read_type_as_pointer (type);

      if (pointer_semantics)
  os << type << "::CDR_Type__ *" << element_name << "_tmp (0)"
     << ";"
     << "std::auto_ptr < " << type << "::CDR_Type__ > "
     << "auto_" << element_name << " (" << element_name << "_tmp);";
      else
  os << type << "::CDR_Type__ " << element_name << "_tmp"
     << ";" ;

      os << "if (" << element_name << "_p)"
   << "{";

      if (pointer_semantics)
      {
  this->optional_attribute_list_.push_back (L"*" +
              element_name +
              L"_tmp");
  // read_pointer_type (element_name, type);
  // execept that we don't create the element
  os << "stream >> " << element_name << "_tmp;";
      }
      else
      {
  this->optional_attribute_list_.push_back (element_name +
              L"_tmp");
  // read_simple_type (element_name, type);
  // execept that we don't create the element
  if (type == L"::XMLSchema::boolean")
    os << "stream.read_boolean";
  else
    os << "stream >>";
  os << " (" << element_name << "_tmp);";
      }

      // post close the bracked
      os << "}";
    }

    /*
     * Function to read any generic type.
     */
    void
    read_generic_type (SemanticGraph::Element &e,
           bool )
    {
      // What we need here is to check
      // for is three cases:
      // Case 1: Type is a new type that is defined
      //         in the schema. Then all we need to
      //         is directly read in the value.
      //
      // Case 2: Type is a simple type, i.e., it
      //         does not have >> and << generated.
      //  Case 2.1: Type inherits from a type that
      //            has to be read as a pointer
      //  Case 2.2: Type inherits from a type that
      //            has to be read as a value directly.
      //
      string type (type_name (e));
      string name (e.name ());

      os << "// " << name << endl;

      SemanticGraph::Type& t (e.type ());
      if (SemanticGraph::Complex* c =
    dynamic_cast<SemanticGraph::Complex*> (&t))
      {
  // Case 1: Inherits from none -- Complex type
  SemanticGraph::Complex::InheritsIterator b (c->inherits_begin ()),
    e (c->inherits_end ());

  if (b == e)
  {
    os << type << " *" << name << " (0);";
    read_complex_type (name, type);
    this->type_name_list_.push_back (L"*" + name);
    return;
  }

  // Inherit from something defined in the schema
  // Case 1: Simple type i.e. no CDR operators define
  // Case 2: Complex type i.e. class has attributes
  //         so this in turn became a comple type and
  //         we generated >> and << operators.

  // Check if the type has an attribute or
  // is an enumeration
  if (has<Traversal::Attribute> (*c) ||
      dynamic_cast<SemanticGraph::Enumeration*> (&t))
  {
    os << type << " *" << name << "(0);";
    read_complex_type (name, type);
    this->type_name_list_.push_back (L"*" + name);
    return;
  }

  // Simple type: However this type is different
  // from a XSC defined type. For example, this type
  // inherits from XSC defined type.
  // class foo : XMLSchema::string < wchar_t >
  // @@ Look at only the immediate predecessor. Is this
  // always enough?
  string base_type (type_name ((**b).inheritee ()));
  if (read_type_as_pointer (base_type))
  {
    os << base_type << "::CDR_Type__ *" << name << "_tmp (0);";
    read_pointer_type (name, base_type);
  }
  else
  {
    os << base_type << "::CDR_Type__ " << name << "_tmp;";
    read_simple_type (name, base_type);
  }

  this->type_name_list_.push_back (name);
      }
      else
      {
  // Simple type: i.e. XMLSchema::boolean, XMLSchema::integer
  if (read_type_as_pointer (type))
  {
    os << type << "::CDR_Type__ *" << name << "_tmp (0);";
    read_pointer_type (name, type);
  }
  else
  {
    os << type << "::CDR_Type__ " << name << "_tmp;";
    read_simple_type (name, type);
  }

  this->type_name_list_.push_back (name);
      }
    }

    /*
     * Read a sequence element.
     */
    void
    read_sequence (SemanticGraph::Element &e)
    {
      // Read length first and then
      // individual elements
      string type (type_name (e));
      string name (e.name ());

      // Pre: read the number of elements
      // in the sequence.
      os << "// read " << name << endl
         << "size_t " << name << "_size;"
         << "stream >> " << name << "_size;"
         << endl
         << "for (size_t " << name << "_counter (0);"
         << name << "_counter < " << name << "_size;"
         << "++ " << name << "_counter)"
         << endl
         << "{" ;

      // Read the generic element type
      read_generic_type (e,0);

      // Post: Add the element to the list
      os << "element->add_" << name;

      // Code to add the id: Now how do we know if
      // type is a simple type or a complex type?
      // the read_generic_type (e) determined that
      // for us. But, here we don't have the type
      // information. Ok! the type_name_list's data
      // item would have the type as we pushed it onto
      // the list!! Use that here..
      std::vector < string >::reverse_iterator iter =
  this->type_name_list_.rbegin ();
      os << " (" << (*iter) << ");"
         << "}";
    }

    //// Acutal Traversal functions ///////////////
    void
    traverse (SemanticGraph::Type &c)
    {

      // These types are defined in the schema as
      // follows:
      // <xsd complex element foo>
      // <xsd:extension name="foobar" type = "xyz>
      // For these types, we directly read in the
      // type from the stream

      string type (type_name (c));

      // We need to transform the element_name to
      // lowercase
      std::transform (element_name_.begin (),
          element_name_.end (),
          element_name_.begin (),
          tolower);

      // read the complex type in
      os << type << "::CDR_Type__ *" << element_name_ << "_tmp;";
      read_pointer_type (element_name_, type);

      // Add this to the list of arguments to construct the
      // element.
      this->type_name_list_.push_back (element_name_);
    }

    virtual void
    traverse (SemanticGraph::Element& e)
    {
      string type (type_name (e));
      string name (e.name ());

      if (e.max () != 1)
      {
  // Sequence: Read sequences after creating
  // the element
  // Safe hack: Memory not managed by container.
  // released elsewhere. So this is safe
  this->complex_list_.push_back (&e);
      }
      else if (e.min () == 0)
      {
  this->optional_element_list_.push_back (&e);
      }
      else
      {
  // Read a generic type
  read_generic_type (e,0);
      }
    }

    // Entry point from which all other types will be
    // visited and traversed
    void
    traverse (SemanticGraph::Complex &c)
    {
      // set the name of the element we are
      // processing. In particular, we need the
      // lowercase. For example, a name could be
      // Title, Car etc. We need car, title
      element_name_  = c.name ();

      complex_.traverse (c);

      // post: Create the element
      string type (type_name (c));

      os << "element = new " << type << " (";
      for (size_t i =0; i < this->type_name_list_.size (); i++)
      {
  os << (i? ", " : "")
     << this->type_name_list_[i];
      }

      os << ");" << endl;

      // Add any sequence/optional attribute elements
      this->finally ();
    }

    virtual void
    traverse (SemanticGraph::Attribute& a)
    {
      string type (type_name (a));
      string name (id (a.name ()));

      os << "// " << name << endl;

      if (!a.optional ())
  read_attribute (name, type);
      else
        // Read in the optional attribute
  read_optional_attribute (name, type);
    }

    void
    has_attributes (bool settings)
    {
      this->has_attributes_ = settings;
    }

    void
    reset_type_attribute_settings ()
    {
      this->has_attributes_ = 0;
      this->complex_list_.clear ();
      this->type_name_list_.clear ();
      this->optional_attribute_list_.clear ();
      this->optional_element_list_.clear ();
    }

    void
    add_attribute (string &name)
    {
      // This attribute should be added only
      // if we read this. so check if we read this
      // attribute
      os << "if (" << name << "_p) "
   << "element->" << name
   << " (" << name << "_tmp);";
    }

    void
    pretty_print_optional_attribute (string &name)
    {
      // Check if the first character is a "*"
      // Indicates the type is a pointer type
      if (name[0] == L'*')
      {
  // Find the position in the string where
  // "_tmp" was added. NOTE: we add _tmp at the
  // end so we need to use rfind and not find.
  // using find might not work!!!
  string::size_type position = name.rfind (L"_tmp");

  // Ignore the leading *
  string attribute_name (name, 1, position-1);
  add_attribute (attribute_name);
      }
      else
      {
  string::size_type position = name.rfind (L"_tmp");
  if (position != string::npos)
  {
    string attribute_name (name, 0, position);
    add_attribute (attribute_name);
  }
  else
    add_attribute (name);
      }
    }

    void
    finally ()
    {
      // Add optional attributes to the element
      if (! this->optional_attribute_list_.empty ())
      {
  for (size_t j =0;
       j < this->optional_attribute_list_.size ();
       j++)
    pretty_print_optional_attribute (this->optional_attribute_list_[j]);
  os << endl;
      }

      // Write the sequence elements
      if (! this->complex_list_.empty ())
  for (size_t i =0; i < this->complex_list_.size (); i++)
    this->read_sequence (*this->complex_list_[i]);

      // NOTE: The optional elements ALWAYS have to be read at
      // the END.
      if (! this->optional_element_list_.empty ())
  for (size_t i = 0; i < this->optional_element_list_.size (); i++)
  {
    string name (this->optional_element_list_[i]->name ());
    // Read the optional part first
    os << "bool " << name << "_p;"
       << "stream.read_boolean (" << name << "_p);";

    os << "if (" << name << "_p)"
       << "{";

    // The element is optional
    read_generic_type (*this->optional_element_list_[i], 1);

    // Add that type to the element
    os << "element->" << name << " (" << name << ");"
       << endl;

    os << "}";
  }
    }

  private:
    Traversal::Complex complex_;
    Traversal::Inherits inherits_;
    Traversal::Names names_;
    bool has_attributes_;

    std::vector< SemanticGraph::Element* > complex_list_;
    std::vector < string > type_name_list_;
    // For complex types, we first read in the types (types
    // and or attributes)
    // that are contained by this type and then add the sequence
    // elements and the attribute list. These two data structures
    // help us in storing the typenames of the sequence elements
    // and their attributes

    std::vector < string > optional_attribute_list_;
    // This list stores all optional attributes in the schema
    // types. These optional attributes similar to sequences
    // will be added to the element after construction of the
    // complex element. However, unlike sequences, they will
    // be read/written in place i.e., in the same order they
    // occur in the schema.

    std::vector < SemanticGraph::Element* > optional_element_list_;
    // This list maintains elements that are defined in the schema
    // of the form:
    // <xs:element name="ContactInfo" type="xs:string"
    // minOccurs="0" maxOccurs="1"/>, where the element is itself
    // optional. These are added to the element after the element
    // is constructed.

    string element_name_;
    // Points to the name of the current element being traversed
    // Used only in the case of simple types that derive from
    // extension base. Is not used in any other case
  };

  //@@ I am using this traverser to handle anonymous types which is
  //   not very clean (and asymmetric).
  //
  struct ComplexElement : Traversal::Complex, protected virtual Context
  {
    ComplexElement (Context& c,
                    Traversal::NodeDispatcher& anonymous_type,
                    string const& name_ = L"")
        : Context (c),
          name (name_),
          element_ (c),
          attribute_ (c),
          cdr_writer_ (c),
          cdr_reader_ (c)
    {
      names_elements_.node_traverser (element_);
      names_attributes_.node_traverser (attribute_);
      names_anonymous_.node_traverser (anonymous_type);
    }


    virtual void
    traverse (Type& c)
    {
      if (c.named ()) 
        name = id (c.name ());

      if (!name.empty ())
      {
        enter_scope (name);

        c.context ().set ("name", name);

        // Go after anonymous types first.
        //
        names (c, names_anonymous_);

        Traversal::Complex::traverse (c);

        c.context ().remove ("name");

        leave_scope ();
      }
    }

    virtual void
    pre (Type& c)
    {
      string type (type_name (c));

      os << "// " << scope << endl
         << "//" << endl
         << endl;

      os << scope << "::" << endl
         << name << " (" << xml_element_type << " const& e)" << endl
         << ":" << "Base (e), regulator__ ()"
         << "{"
         << endl;
    }

    using Traversal::Complex::names;

    virtual void
    names (Type& c)
    {
      bool he (has<Traversal::Element> (c));
      bool ha (has<Traversal::Attribute> (c));

      if (he || ha)
      {
        os << parser_type << " p (e);"
           << endl;

        if (he)
        {
          os << "while (p.more_elements ())"
             << "{"
             << xml_element_type << " e (p.next_element ());"
             << string_type << " n (::XSCRT::XML::uq_name (e.name ()));"
             << endl;

          names (c, names_elements_);

          os << "{}"        // else block
             << "}";        // while loop
        }

        if (ha)
        {
          os << "while (p.more_attributes ())"
             << "{"
             << xml_attribute_type << " a (p.next_attribute ());"
             << string_type << " n (::XSCRT::XML::uq_name (a.name ()));";

          names (c, names_attributes_);

          os << "{}"        // else block
             << "}";        // while loop
        }
      }
    }

    virtual void
    post (Type& )
    {
      os << "}";
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

    virtual void
    insertion_extraction (Type &c)
    {
      // Check if one needs to generate read/write operations
      // for this type
      if (! generate_cdr_types (c))
        return;

      //write all elements contained in this type
      if (this->cdr_writer_generation_)
      {

        // pre
        os << "// write " << name << endl;
        os << "bool " << endl << name << "::write_" << name
           << " (::XMLSchema::CDR_OutputStream &stream) const"
           << "{";

        {
          // Check if the type has any attributes. If so then
          // if the type also has sequences, we write the
          // attributes before writing the sequence elements.
          // This is because, at the recipient, the type can be
          // reconstructed before adding the sequences eliminating
          // temporary storage
          this->cdr_writer_.has_attributes (has<Traversal::Attribute> (c));
          this->cdr_writer_.traverse (c);
          this->cdr_writer_.reset_type_attribute_settings ();
        }

        // post
        os << "return stream.good_bit ();";
        os << "}" << endl;
      }

      // Read the written types from the stream
      if (this->cdr_reader_generation_)
      {
        // pre
        os << "// read " << name << endl;
        os << "bool" << endl << name << "::" << endl
           << "read_" << name
           << " (::XMLSchema::CDR_InputStream &stream," << endl;

        for (size_t j =0; j < name.length () + 5; j++)
          os << " ";

        os << "::XMLSchema::cdr_arg_traits < " << name
           << " >::inout_type element)"
           << "{";

        {
          this->cdr_reader_.has_attributes (has<Traversal::Attribute> (c));
          this->cdr_reader_.traverse (c);
          this->cdr_reader_.reset_type_attribute_settings ();
        }

        // post
        os << "return stream.good_bit ();";
        os << "}" << endl;
      }

    }

  private:
    string name;

    Traversal::Names names_elements_;
    Traversal::Names names_attributes_;
  
    Element element_;
    Attribute attribute_;

    Traversal::Names names_anonymous_;

    CDR_WriterTraverser cdr_writer_;
    CDR_ReaderTraverser cdr_reader_;
  };

  struct ComplexAttribute : Traversal::Complex, protected virtual Context
  {
    ComplexAttribute (Context& c, string const name_ = L"")
        : Context (c), name (name_)
    {
    }

    virtual void
    traverse (Type& c)
    {
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
          if (has<Traversal::Element> (c) ||
              has<Traversal::Attribute> (c)) v_ = false;
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
        if (c.named ()) name = id (c.name ());

        enter_scope (name);

        c.context ().set ("name", scope);

        Traversal::Complex::traverse (c);

        c.context ().remove ("name");

        leave_scope ();
      }
    }


    virtual void
    pre (Type&)
    {
      os << scope << "::" << endl
         << name << " (" << xml_attribute_type << " const& a)" << endl
         << ":" << endl
         << "Base (a)," << endl
         << "regulator__ ()"
         << "{";
    }

    virtual void
    post (Type&)
    {
      os << "}";
    }

  private:
    string name;
  };

  struct StaticEnumerator : Traversal::Enumerator, protected virtual Context
  {
    StaticEnumerator (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      string name (id (e.name ()));

      bool anon (!e.type ().named ());

      os << (anon ? "class " : "" ) << scope << " const " <<  scope << "::"
         << name << " (" << scope << "::" << name << "_l);";
    }
  };

  struct Enumerator : Traversal::Enumerator, protected virtual Context
  {
    Enumerator (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      string name (e.name ());

      os << "if (v == " << L << "\"" << name << "\") v_ = " << id (name) << "_l;"
         << "else ";
    }
  };


  struct Enumeration : Traversal::Enumeration, protected virtual Context
  {
    Enumeration (Context& c, string name_ = L"")
        : Context (c), name (name_), enumerator_ (c), static_enumerator_ (c)
    {
      names_.node_traverser (enumerator_);
      names_static_.node_traverser (static_enumerator_);
    }

    virtual void
    traverse (Type& e)
    {
      if (e.named ()) name = id (e.name ());

      enter_scope (name);

      os << "// " << scope << endl
         << "//" << endl
         << endl;

      // c-tor (Element const&)
      //
      os << scope << "::" << endl
         << name << " (" << xml_element_type << " const& e)" << endl
         << ": ::XSCRT::Type (e)" << endl
         << "{"
         << string_type << " v (e.value ());"
         << endl;

      names (e, names_);

      os << "{}" // else block
         << "}";

      // c-tor (Attribute const&)
      //
      os << scope << "::" << endl
         << name << " (" << xml_attribute_type << " const& a)" << endl
         << ": ::XSCRT::Type (a)" << endl
         << "{"
         << string_type << " v (a.value ());"
         << endl;

      names (e, names_);

      os << "{}" // else block
         << "}";

      names (e, names_static_);

      os << endl;

      // CDR reading/writing if enabled
      if (this->cdr_writer_generation_)
      {
  // pre
  os << "// write " << name << endl;
  os << "bool " << endl << name << "::write_" << name
     << " (::XMLSchema::CDR_OutputStream &stream) const"
     << "{";

  // For enumerations marshal them using the
  // corresponding integer value
        os << "// " << name << endl;
  os << "int " << name << "_integral = "
     << "this->integral ();"
     << "stream << " << name << "_integral;"
     << endl;

  // post
  os << "return stream.good_bit ();" << endl;
  os << "}" << endl;
      }

      // CDR reading/writing if enabled
      if (this->cdr_reader_generation_)
      {
  // pre
  os << "// read " << name << endl;
  os << "bool " << endl << name << "::read_" << name
     << " (::XMLSchema::CDR_InputStream &stream, "
     << endl
     << "::XMLSchema::cdr_arg_traits < " << name
     << " >::inout_type element)" << endl
     << "{";

  // For enumerations demarshal them via the
  // corresponding integer value
  string type (type_name (e));
  os << "int " << name << "_tmp;"
     << "stream >> " << name << "_tmp;"
     << "element = new " << type << " (Value ("
     << name << "_tmp));";

  // post
  os << "return stream.good_bit ();"
     << "}"
     << endl;
      }

      leave_scope ();
    }

  private:
    string name;

    Traversal::Names names_;
    Enumerator enumerator_;

    Traversal::Names names_static_;
    StaticEnumerator static_enumerator_;
  };


  struct AnonymousType : Traversal::Element, protected virtual Context
  {
    AnonymousType (Context& c)
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

        Traversal::Belongs belongs;
        ComplexElement complex_element (*this, *this, name);
        ComplexAttribute complex_attribute (*this, name);
        Enumeration enumeration (*this, name);

        belongs.node_traverser (complex_element);
        belongs.node_traverser (complex_attribute);
        belongs.node_traverser (enumeration);

        t.context ().set ("seen", true);

        Element::belongs (e, belongs);

        t.context ().remove ("seen");
      }
    }
  };
}

void
generate_source (Context& ctx, SemanticGraph::Schema& schema)
{
  Traversal::Schema traverser;
  Traversal::Sources sources;
  Traversal::Names schema_names;
  Namespace ns (ctx);

  traverser.edge_traverser (sources);
  traverser.edge_traverser (schema_names);
  sources.node_traverser (traverser);
  schema_names.node_traverser (ns);

  Traversal::Names names;
  AnonymousType anonymous_type (ctx);
  ComplexElement complex_element (ctx, anonymous_type);
  ComplexAttribute complex_attribute (ctx);
  Enumeration enumeration (ctx);

  ns.edge_traverser (names);
  names.node_traverser (enumeration);
  names.node_traverser (complex_element);
  names.node_traverser (complex_attribute);
  names.node_traverser (anonymous_type);

  traverser.traverse (schema);
}
