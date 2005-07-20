// file      : CXX/Validator.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <CXX/Validator.hpp>
#include <CXX/Elements.hpp>

#include <XSC/SemanticGraph.hpp>
#include <XSC/Traversal.hpp>

#include <CCF/CodeGenerationKit/Regex.hpp>

using std::endl;
using std::wcerr;

namespace
{
  //@! Inheriting from Context is just wrong!
  //
  class ValidationContext : public Context
  {
  public:
    ValidationContext (bool& valid_)
        : Context (std::wcout,
                   L"",
                   L"",
                   NamespaceMapping (),
                   L"anonymous type for "),
          valid (valid_)
    {
    }

  protected:
    ValidationContext (ValidationContext& c)
        : Context (c), valid (c.valid)
    {
    }

  protected:
    bool& valid;
  };


  struct Element : Traversal::Element, protected virtual ValidationContext
  {
    Element (ValidationContext& c)
        : ValidationContext (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      SemanticGraph::Type& t (e.type ());

      if (dynamic_cast<SemanticGraph::AnyType*> (&t))
      {
        valid = false;

        wcerr << "element `" << scope << "::" << e.name ()
              << "' is of type `anyType'" << endl
              << "`anyType' is not supported in this mapping" << endl
              << "did you forget to specify `type' attribute?" << endl;
      }
    }
  };

  struct Attribute : Traversal::Attribute, protected virtual ValidationContext
  {
    Attribute (ValidationContext& c)
        : ValidationContext (c)
    {
    }

    virtual void
    traverse (Type& a)
    {
      SemanticGraph::Type& t (a.type ());

      if (dynamic_cast<SemanticGraph::AnySimpleType*> (&t))
      {
        valid = false;

        wcerr << "attribute `" << scope << "::" << a.name ()
              << "' is of type `anySimpleType'" << endl
              << "`anySimpleType' is not supported in this mapping" << endl
              << "did you forget to specify `type' attribute?" << endl;
      }
    }
  };

  struct Complex : Traversal::Complex,
                   Traversal::Type,
                   protected virtual ValidationContext
  {
    Complex (ValidationContext& c)
        : ValidationContext (c), element_ (c), attribute_ (c)
    {
      edge_traverser (names_);
      names_.node_traverser (element_);
      names_.node_traverser (attribute_);
    }

    virtual void
    traverse (SemanticGraph::Complex& c)
    {
      traverse (static_cast<SemanticGraph::Type&> (c));

      //@@ maybe use traverser?
      //
      SemanticGraph::Complex::InheritsIterator it (c.inherits_begin ());

      if (it != c.inherits_end ())
      {
        SemanticGraph::Type& t ((**it).inheritee ());

        if (types_.find (t.scope ().name () + L"#" + t.name ()) == types_.end ())
        {
          valid = false;

          string name (type_name (c));

          wcerr << "type `" << name << "' inherits from a yet undefined "
                << "type `" << type_name (t) << "'" << endl
                << "inheritance from yet-undefined types is not supported in "
                << "this mapping" << endl
                << "re-arrange your schema and try again" << endl;
        }
      }

      Traversal::Complex::traverse (c);
    }

    virtual void
    traverse (SemanticGraph::Type& t)
    {
      if (t.named ())
      {
        types_.insert (t.scope ().name () + L"#" + t.name ());
      }
    }

  private:
    std::set<string> types_;

    Element element_;
    Attribute attribute_;

    Traversal::Names names_;
  };
}

namespace CXX
{
  bool
  validate (SemanticGraph::Schema& schema)
  {
    bool valid (true);

    ValidationContext ctx (valid);

    Traversal::Schema traverser;

    Traversal::Implies implies; // go to XMLSchema namespace
    Traversal::Sources sources;
    Traversal::Includes includes;
    Traversal::Includes imports;

    Traversal::Names schema_names;

    Traversal::Namespace ns;

    traverser.edge_traverser (implies);
    traverser.edge_traverser (sources);
    traverser.edge_traverser (includes);
    traverser.edge_traverser (imports);
    traverser.edge_traverser (schema_names);

    implies.node_traverser (traverser);
    sources.node_traverser (traverser);
    includes.node_traverser (traverser);
    imports.node_traverser (traverser);
    schema_names.node_traverser (ns);

    Traversal::Names names;
    Complex complex (ctx);
    Element element (ctx);
    Attribute attribute (ctx);
    Anonymous anonymous (ctx, complex);

    ns.edge_traverser (names);
    names.node_traverser (complex);
    names.node_traverser (anonymous);
    names.node_traverser (element);
    names.node_traverser (attribute);

    traverser.traverse (schema);

    return valid;
  }

}
