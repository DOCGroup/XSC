// file      : XSC/Parser.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <XSC/Parser.hpp>
#include <XSC/SemanticGraph.hpp>
#include <XSC/Traversal.hpp>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/validators/schema/XSDDOMParser.hpp>

#include <xercesc/sax/InputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#include <cstdlib>  // std::wcstombs
#include <memory>   // std::auto_ptr
#include <iostream>

using std::wcout;
using std::wcerr;
using std::endl;

namespace XSC
{
  bool trace_ = false;

  using namespace SemanticGraph;
  using namespace Xerces;
  using std::auto_ptr;

  string const xsd = L"http://www.w3.org/2001/XMLSchema";
  string const xmi = L"http://www.omg.org/XMI";
  
  namespace
  {
    class NotNamespace {};
    class NotName {};

    template <typename T>
    T&
    resolve (string const& ns_name,
             string const& uq_name,
             Schema& schema)
    {
      Scope::NamesIteratorPair nss (schema.find (ns_name));

      if (nss.first == nss.second)
      {
        throw NotNamespace ();
      }

      for (; nss.first != nss.second; ++nss.first)
      {
        Namespace& ns (dynamic_cast<Namespace&> ((*nss.first)->named ()));

        Scope::NamesIteratorPair types (ns.find (uq_name));

        if (types.first != types.second)
        {
          T& t (dynamic_cast<T&> ((*types.first)->named ()));

          if (trace_)
            wcout << "successfully resolved `" << ns_name << '#' << uq_name
                  << "'" << endl;

          return t;
        }
      }

      throw NotName ();
    }

    struct Resolver : Traversal::Instance,
                      Traversal::Complex,
                      Traversal::Enumeration,
                      Traversal::Scope // group
    {
      Resolver (Schema& s)
        : schema_ (s)
      {
      }

      void
      traverse (SemanticGraph::Instance& i)
      {
        string ns_name;
        string uq_name;

        try
        {
          if (i.context ().count ("type-ns-name"))
          {
            ns_name = i.context ().get<string> ("type-ns-name");
            uq_name = i.context ().get<string> ("type-uq-name");

            schema_.new_edge<Belongs> (
              i, resolve<SemanticGraph::Type> (ns_name, uq_name, schema_));

            i.context ().remove ("type-ns-name");
            i.context ().remove ("type-uq-name");
          }
          else if (i.context ().count ("instance-ns-name"))
          {
            ns_name = i.context ().get<string> ("instance-ns-name");
            uq_name = i.context ().get<string> ("instance-uq-name");

            SemanticGraph::Instance& ref (
              resolve<SemanticGraph::Instance> (ns_name, uq_name, schema_));

            if (ref.typed ())
            {
              schema_.new_edge<Belongs> (i, ref.type ());

              i.context ().remove ("instance-ns-name");
              i.context ().remove ("instance-uq-name");
            }
            else
            {
              wcerr << "referenced instance " << ns_name << "#"
                    << uq_name << " is not typed" << endl;
            }
          }
        }
        catch (NotNamespace const&)
        {
          wcerr << "unable to resolve namespace `" << ns_name << "'" << endl;
        }
        catch (NotName const&)
        {
          wcerr << "unable to resolve name `" << uq_name
                << "\' inside namespace `" << ns_name << "'" <<endl;
        }
      }

      void
      traverse (SemanticGraph::Complex& c)
      {
        string ns_name;
        string uq_name;

        try
        {
          if (c.context ().count ("type-ns-name"))
          {
            ns_name = c.context ().get<string> ("type-ns-name");
            uq_name = c.context ().get<string> ("type-uq-name");

            schema_.new_edge<Inherits> (c, resolve<SemanticGraph::Type> (ns_name, uq_name, schema_));

            c.context ().remove ("type-ns-name");
            c.context ().remove ("type-uq-name");
          }
          else if (c.context ().count ("group-ns-name"))
          {
            ns_name = c.context ().get<string> ("group-ns-name");
            uq_name = c.context ().get<string> ("group-uq-name");

            unsigned long min (c.context ().get<unsigned long> ("group-min")),
              max (c.context ().get<unsigned long> ("group-max"));

            Scope& scope (resolve<Scope> (ns_name, uq_name, schema_));

            // @@ same code as in Parser::group
            //
            for (Scope::NamesIterator i (scope.names_begin ());
                 i != scope.names_end (); ++i)
            {
              Element& prot (dynamic_cast<Element&> ((*i)->named ()));

              Element& e (schema_.new_node<Element> (
                            min == 0 ? min : prot.min (),
                            max == 0 ? max : prot.max (),
                            prot.qualified ()));

              schema_.new_edge<Names> (c, e, prot.name ());

              // Now we set element's type.
              //
              //
              if (prot.typed ())
              {
                // Easy case.
                //
                schema_.new_edge<Belongs> (e, prot.type ());
              }
              else if (prot.context ().count ("type-ns-name"))
              {
                string ns_name (prot.context ().get<string> ("type-ns-name"));
                string uq_name (prot.context ().get<string> ("type-uq-name"));

                e.context ().set ("type-ns-name", ns_name);
                e.context ().set ("type-uq-name", uq_name);
              }
              else if (prot.context ().count ("instance-ns-name"))
              {
                string ns_name (prot.context ().get<string> ("instance-ns-name"));
                string uq_name (prot.context ().get<string> ("instance-uq-name"));

                e.context ().set ("instance-ns-name", ns_name);
                e.context ().set ("instance-uq-name", uq_name);
              }
              else
              {
                // What the heck?
                //
                if (trace_)
                  wcout << "element `" << ns_name << "#" << uq_name
                        << "' is in unexpected condition" << endl;
              }
            }

            c.context ().remove ("group-ns-name");
            c.context ().remove ("group-uq-name");
            c.context ().remove ("group-min");
            c.context ().remove ("group-max");
          }
        }
        catch (NotNamespace const&)
        {
          wcerr << "unable to resolve namespace `" << ns_name << "'" << endl;
        }
        catch (NotName const&)
        {
          wcerr << "unable to resolve name `" << uq_name
                << "\' inside namespace `" << ns_name << "'" <<endl;
        }

        Traversal::Complex::traverse (c);
      }

      // We don't need to go inside enumerations.
      //
      void
      traverse (SemanticGraph::Enumeration&)
      {
      }

    private:
      Schema& schema_;
    };
  }

  Parser::
  Parser (bool trace)
      : root_schema_ (0), cur_schema_ (0), qualify_attribute_ (false), qualify_element_ (false)
  {
    trace_ = trace;
  }

  Schema* Parser::
  parse (fs::path const& uri)
  {
    auto_ptr<Schema> rs (new Schema);

    // Implied schema with fundamental types.
    //
    {
      Schema& s (rs->new_node<Schema> ());
      rs->new_edge<Implies> (*rs, s, fs::path ("XMLSchema.xsd"));

      Namespace& ns (s.new_node<Namespace> ());
      s.new_edge<Names> (s, ns, xsd);

      s.new_edge<Names> (ns, s.new_node<AnyType> (), L"anyType");
      s.new_edge<Names> (ns, s.new_node<AnySimpleType> (), L"anySimpleType");

      s.new_edge<Names> (ns, s.new_node<Byte> (), L"byte");
      s.new_edge<Names> (ns, s.new_node<UnsignedByte> (), L"unsignedByte");
      s.new_edge<Names> (ns, s.new_node<Short> (), L"short");
      s.new_edge<Names> (ns, s.new_node<UnsignedShort> (), L"unsignedShort");
      s.new_edge<Names> (ns, s.new_node<Int> (), L"int");
      s.new_edge<Names> (ns, s.new_node<UnsignedInt> (), L"unsignedInt");
      s.new_edge<Names> (ns, s.new_node<Long> (), L"long");
      s.new_edge<Names> (ns, s.new_node<UnsignedLong> (), L"unsignedLong");

      s.new_edge<Names> (ns, s.new_node<Decimal> (), L"decimal");
      s.new_edge<Names> (ns, s.new_node<Integer> (), L"integer");
      s.new_edge<Names> (ns, s.new_node<NonPositiveInteger> (), L"nonPositiveInteger");
      s.new_edge<Names> (ns, s.new_node<NonNegativeInteger> (), L"nonNegativeInteger");
      s.new_edge<Names> (ns, s.new_node<PositiveInteger> (), L"positiveInteger");
      s.new_edge<Names> (ns, s.new_node<NegativeInteger> (), L"negativeInteger");

      s.new_edge<Names> (ns, s.new_node<Boolean> (), L"boolean");

      s.new_edge<Names> (ns, s.new_node<Float> (), L"float");
      s.new_edge<Names> (ns, s.new_node<Double> (), L"double");

      s.new_edge<Names> (ns, s.new_node<String> (), L"string");

      s.new_edge<Names> (ns, s.new_node<NormalizedString> (), L"normalizedString");
      s.new_edge<Names> (ns, s.new_node<Token> (), L"token");
      s.new_edge<Names> (ns, s.new_node<Name> (), L"Name");
      s.new_edge<Names> (ns, s.new_node<NMTOKEN> (), L"NMTOKEN");
      s.new_edge<Names> (ns, s.new_node<NCName> (), L"NCName");

      s.new_edge<Names> (ns, s.new_node<Id> (), L"ID");
      s.new_edge<Names> (ns, s.new_node<IdRef> (), L"IDREF");
    }
    
    // Supported XMI features
    {
      Schema &s (rs->new_node<Schema> ());
      rs->new_edge<Implies> (*rs, s, fs::path ("XMI.xsd"));
      Namespace &ns (s.new_node<Namespace> ());
      
      s.new_edge<Names> (s, ns, xmi);
      
      s.new_edge<Names> (ns, s.new_node<String> (), L"href");
    }
    
    // Parse.
    //
    if (DOMDocument* d  = dom (uri))
    {
      XML::Element root (d->getDocumentElement ());

      string ns (root[L"targetNamespace"]);

      if (trace_) wcout << "target namespace: " << ns << endl;

      // Enter the file into file_map_.
      //
      file_map_[uri] = ns;

      root_schema_ = cur_schema_ = rs.get ();

      push_scope (root_schema_->new_node<Namespace> ());
      root_schema_->new_edge<Names> (*cur_schema_, scope (), ns);

      schema (root);

      pop_scope ();

      root_schema_ = cur_schema_ = 0;
    }

    //@@ don't need this in case of an error
    //

    // Second pass to resolve types and refs.
    //
    {
      Traversal::Schema schema;
      Traversal::Contains contains;
      Traversal::Names schema_names;
      Traversal::Namespace ns;

      schema.edge_traverser (contains);
      schema.edge_traverser (schema_names);
      schema_names.node_traverser (ns);
      contains.node_traverser (schema);

      Traversal::Names ns_names;
      Resolver resolver (*rs);

      struct AnonymousType : Traversal::Instance
      {
        AnonymousType (Traversal::NodeDispatcher& scope)
        {
          belongs_.node_traverser (scope);
        }

        virtual void
        traverse (Type& e)
        {
          if (e.typed () &&
              !e.type ().named () &&
              !e.context ().count ("seen"))
          {
            e.context ().set ("seen", true);

            belongs (e, belongs_);

            e.context ().remove ("seen");
          }
        }

      private:
        Traversal::Belongs belongs_;

      } anonymous_type (resolver);

      ns.edge_traverser (ns_names);

      ns_names.node_traverser (anonymous_type);
      ns_names.node_traverser (resolver);

      Traversal::Names names;
      resolver.edge_traverser (names);
      names.node_traverser (anonymous_type);
      names.node_traverser (resolver);

      if (trace_)
        wcout << "post-resolution pass #1" << endl;

      schema.traverse (*rs);
    }

    return rs.release ();
  }

  void Parser::
  schema (XML::Element const& s)
  {
    bool old_qa (qualify_attribute_);
    bool old_qe (qualify_element_);

    if (string af = s[L"attributeFormDefault"])
    {
      qualify_attribute_ = af == L"qualified";
    }

    if (string ef = s[L"elementFormDefault"])
    {
      qualify_element_ = ef == L"qualified";
    }

    push (s);

    while (more ())
    {
      XML::Element e (next ());
      string name (e.name ());

      if (trace_) wcout << name << endl;

      if (name == L"annotation"); else
      if (name == L"import") import (e); else
      if (name == L"include") include (e); else
      if (name == L"group") group (e); else
      if (name == L"simpleType") simple_type (e); else
      if (name == L"complexType") complex_type (e); else
      if (name == L"element") element (e, true); else
      {
        wcerr << "unexpected top-level element: " << name << endl;
      }
    }

    pop ();

    qualify_attribute_ = old_qa;
    qualify_element_ = old_qe;
  }

  void Parser::
  import (XML::Element const& i)
  {
    string loc (i[L"schemaLocation"]);

    size_t len (std::wcstombs (0, loc.c_str (), 0) + 1);
    char* str (new char[len]);
    std::wcstombs (str, loc.c_str (), len);
    fs::path path (str);
    delete[] str;
    
    // Check to see if we have already parsed this file....
    if (file_map_.find (path) != file_map_.end ()) return;
    
    
    file_map_[path] = i[L"namespace"];

    if (trace_) wcout << "importing " << path.string ().c_str () << endl;

    if (DOMDocument* d  = dom (path))
    {
      Schema& s (root_schema_->new_node<Schema> ());
      root_schema_->new_edge<Imports> (*cur_schema_, s, path);

      XML::Element root (d->getDocumentElement ());

      string ns (root[L"targetNamespace"]);

      if (trace_) wcout << "target namespace: " << ns << endl;

      Schema* old (cur_schema_);
      cur_schema_ = &s;

      push_scope (root_schema_->new_node<Namespace> ());
      root_schema_->new_edge<Names> (*cur_schema_, scope (), ns);

      schema (root);

      pop_scope ();
      cur_schema_ = old;
    }
  }

  void Parser::
  include (XML::Element const& i)
  {
    string loc (i[L"schemaLocation"]);

    size_t len (std::wcstombs (0, loc.c_str (), 0) + 1);
    char* str (new char[len]);
    std::wcstombs (str, loc.c_str (), len);
    fs::path path (str);
    delete[] str;

    if (file_map_.find (path) != file_map_.end ()) return;

    file_map_[path] = L"not in use right now";

    if (trace_) wcout << "including " << path.string ().c_str () << endl;

    if (DOMDocument* d  = dom (path))
    {
      Schema& s (root_schema_->new_node<Schema> ());

      XML::Element root (d->getDocumentElement ());

      string ns (root[L"targetNamespace"]), cur_ns;

      if (ns.empty () &&
          !(cur_ns = (*(cur_schema_->names_begin ()))->name ()).empty ())
      {
        // Chameleon.
        //
	// @@Will:  What is a chameleon schema?
        ns = cur_ns;
        root_schema_->new_edge<Sources> (*cur_schema_, s, path);

        if (trace_) wcout << "handling chameleon schema" << endl;
      }
      else
      {
        root_schema_->new_edge<Includes> (*cur_schema_, s, path);
      }

      if (trace_) wcout << "target namespace: " << ns << endl;

      Schema* old (cur_schema_);
      cur_schema_ = &s;

      push_scope (root_schema_->new_node<Namespace> ());
      root_schema_->new_edge<Names> (*cur_schema_, scope (), ns);

      schema (root);

      pop_scope ();
      cur_schema_ = old;
    }
  }

  void Parser::
  group (XML::Element const& g)
  {
    if (string name = g[L"name"])
    {
      Scope& group (root_schema_->new_node<Scope> ());
      root_schema_->new_edge<Names> (scope (), group, name);

      push_scope (group);
      push (g);

      annotation ();

      XML::Element e (next ());

      name = e.name ();

      if (trace_) wcout << name << endl;

      if (name == L"all") all (e); else
      if (name == L"choice") choice (e); else
      if (name == L"sequence") sequence (e); else
      {
        wcerr << "expected `all' or `choice' or `sequence' instead of " << name
              << endl;
      }

      pop ();
      pop_scope ();
    }
    else if (string ref = g[L"ref"])
    {
      string uq_name (XML::uq_name (ref));
      string ns_name (XML::ns_name (g, ref));

      //@@ min/max
      //
      // min/max can come from element in the group!

      unsigned long min (g[L"minOccurs"] == L"0" ? 0 : this->min ()),
        max (g[L"maxOccurs"] && g[L"maxOccurs"] != L"1" ? 0 : this->max ());

      if (trace_) wcout << "group min " << min << " max " << max << endl;


      try
      {
        Scope& s (resolve<Scope> (ns_name, uq_name, *root_schema_));

        for (Scope::NamesIterator i (s.names_begin ());
             i != s.names_end (); ++i)
        {
          Element& prot (dynamic_cast<Element&> ((*i)->named ()));

          Element& e (root_schema_->new_node<Element> (
                        min == 0 ? min : prot.min (),
                        max == 0 ? max : prot.max (),
                        prot.qualified ()));

          root_schema_->new_edge<Names> (scope (), e, prot.name ());

          // Now we set element's type.
          //
          if (prot.typed ())
          {
            // Easy case.
            //
            root_schema_->new_edge<Belongs> (e, prot.type ());
          }
          else if (prot.context ().count ("type-ns-name"))
          {
            string ns_name (prot.context ().get<string> ("type-ns-name"));
            string uq_name (prot.context ().get<string> ("type-uq-name"));

            e.context ().set ("type-ns-name", ns_name);
            e.context ().set ("type-uq-name", uq_name);
          }
          else if (prot.context ().count ("instance-ns-name"))
          {
            string ns_name (prot.context ().get<string> ("instance-ns-name"));
            string uq_name (prot.context ().get<string> ("instance-uq-name"));

            e.context ().set ("instance-ns-name", ns_name);
            e.context ().set ("instance-uq-name", uq_name);
          }
          else
          {
            // What the heck?
            //
            if (trace_)
              wcout << "element `" << ref << "' is in unexpected condition"
                    << endl;
          }
        }
      }
      catch (NotNamespace const&)
      {
        wcerr << "unable to resolve namespace `" << ns_name << "'" << endl;
      }
      catch (NotName const&)
      {
        if (trace_)
          wcout << "unable to resolve group name `" << uq_name
                << "' inside namespace `" << ns_name << "'" << endl
                << "deferring resolution until later" << endl;

        // Cannot resolve group at this point. Will have to try later.
        // The tricky part is that we have to save position where we will
        // be inserting elements.
        //

        scope ().context ().set ("group-ns-name", ns_name);
        scope ().context ().set ("group-uq-name", uq_name);
        scope ().context ().set ("group-min", min);
        scope ().context ().set ("group-max", max);

        /*
          @@ This is too tricky for current stage of implementation.
             What if there are two groups at the beginning?

        if (scope ().names_begin () == scope ().names_end ())
        {
          scope ().context ().set ("group-position-begin", true);
        }
        else
        {
          scope ().context ().set ("group-position-iterator",
                                   scope ().names_end () - 1);
        }
        */
      }
    }
    else
    {
      wcerr << "`name' or `ref' attribute is missing for group declaration"
            << endl;
      return;
    }
  }

  //@@ need RAII for push/pop
  //

  Type* Parser::
  simple_type (XML::Element const& t)
  {
    Type* r (0);

    push (t);

    annotation ();

    XML::Element e (next ());

    string name (e.name ());

    if (name == L"restriction") r = restriction (e); else
    {
      wcerr << "expected `restriction' instead of " << name
            << endl;
    }

    pop ();

    return r;
  }

  Type* Parser::
  restriction (XML::Element const& r)
  {
    Type* rv (0);

    string base (fq_name (r, r[L"base"]));

    if (trace_) wcout << "restriction base: " << base << endl;

    push (r);

    annotation ();

    bool enum_ (false);

    if (more ())
    {
      XML::Element e (next ());

      if (e.name () == L"enumeration")
      {
        // Enumeration
        //
        enum_ = true;

        Enumeration& node (root_schema_->new_node<Enumeration> ());
        set_type<Inherits> (r[L"base"], r, node);

        if (string name = r.parent ()[L"name"])
        {
          root_schema_->new_edge<Names> (scope (), static_cast<Nameable&> (node), name);
        }

        push_scope (node);

        for (enumeration (e); more (); enumeration (next ()));

        pop_scope ();

        rv = &node;
      }
    }

    if (!enum_)
    {
      Complex& node (root_schema_->new_node<Complex> ());
      set_type<Inherits> (r[L"base"], r, node);

      if (string name = r.parent ()[L"name"])
      {
        root_schema_->new_edge<Names> (scope (), node, name);
      }

      rv = &node;
    }

    pop ();

    return rv;
  }

  void Parser::
  enumeration (XML::Element const& e)
  {
    string value (e[L"value"]);

    if (trace_) wcout << "enumeration value: " << value << endl;

    Enumerator& node (root_schema_->new_node<Enumerator> ());
    root_schema_->new_edge<Names> (scope (), node, value);
    root_schema_->new_edge<Belongs> (node, dynamic_cast<Type&>(scope ()));
  }

  Type* Parser::
  complex_type (XML::Element const& t)
  {
    Type* r (0);

    if (t[L"mixed"] == L"true")
    {
      wcerr << "mixed content model is not supported" << endl;
      return r;
    }

    Complex& node (root_schema_->new_node<Complex> ());

    if (string name = t[L"name"])
    {
      root_schema_->new_edge<Names> (scope (), node, name);
    }

    r = &node;

    push_scope (node);
    push (t);

    annotation ();

    XML::Element e (next ());

    string name = e.name ();

    if (trace_) wcout << name << endl;

    if (name == L"group") group (e); else
    if (name == L"all") all (e); else
    if (name == L"choice") choice (e); else
    if (name == L"sequence") sequence (e); else
    if (name == L"attribute") attribute (e); else
    if (name == L"simpleContent") simple_content (e); else
    if (name == L"complexContent") complex_content (e); else
    {
      wcerr << "expected `choice' or `sequence' instead of " << name << endl;
      return r;
    }

    while (more ())
    {
      e = next ();

      if (e.name () == L"attribute") attribute (e); else
      {
        wcerr << "expected `attribute' instead of " << e.name () << endl;
        return r;
      }
    }

    pop ();
    pop_scope ();

    return r;
  }

  void Parser::
  all (XML::Element const& a)
  {
    // `all' cannot be nested inside `choice' or `sequence', nor
    //  can it contain any of those. The default cardinality
    //  values for `all' are min=1, max=1. Therefore we don't
    //  really need to push/pop cardinality.
    //
    this->push (a);

    this->annotation ();
    
    // The content of the all must be a number of elements.  
    while (this->more ())
    {
      XML::Element e (this->next ());

      string name (e.name ());

      if (name == L"element") element (e); else
      {
        wcerr << "expected `element' instead of " << name << endl;
      }
    }

    pop ();
  }

  void Parser::
  choice (XML::Element const& c)
  {
    // Establish cardinality.
    // All choice elements have a min cardinality of 0
    push_cardinality (0,
                      c[L"maxOccurs"] && c[L"maxOccurs"] != L"1" ? 0 : max ());


    push (c);

    while (more ())
    {
      XML::Element e (next ());

      string name (e.name ());

      if (name == L"group") group (e); else
      if (name == L"choice") choice (e); else
      if (name == L"sequence") sequence (e); else
      if (name == L"element") element (e); else
      {
        wcerr << "expected `choice' or `sequence' or `element' instead of "
              << name << endl;
      }
    }

    pop ();

    pop_cardinality ();
  }

  void Parser::
  sequence (XML::Element const& s)
  {
    // Establish cardinality.
    //
    push_cardinality (s[L"minOccurs"] == L"0" ? 0 : min (),
                      s[L"maxOccurs"] && s[L"maxOccurs"] != L"1" ? 0 : max ());

    push (s);

    while (more ())
    {
      XML::Element e (next ());

      string name (e.name ());

      if (name == L"group") group (e); else
      if (name == L"choice") choice (e); else
      if (name == L"sequence") sequence (e); else
      if (name == L"element") element (e); else
      {
        wcerr << "expected `choice' or `sequence' or `element' instead of "
              << name << endl;
      }
    }

    pop ();

    pop_cardinality ();
  }

  void Parser::
  simple_content (XML::Element const& c)
  {
    push (c);

    XML::Element e (next ());
    string name (e.name ());

    if (name == L"extension") simple_content_extension (e); else
    {
      wcerr << "expected `extension' instead of " << name << endl;
      return;
    }

    pop ();
  }

  void Parser::
  complex_content (XML::Element const& c)
  {
    if (c[L"mixed"] == L"true")
    {
      wcerr << "mixed content model is not supported" << endl;
      return;
    }

    push (c);

    XML::Element e (next ());
    string name (e.name ());

    if (name == L"extension") complex_content_extension (e); else
    {
      wcerr << "expected `extension' instead of " << name << endl;
      return;
    }

    pop ();
  }

  void Parser::
  simple_content_extension (XML::Element const& e)
  {
    if (trace_) wcout << "extension base: " << fq_name (e, e[L"base"]) << endl;

    set_type<Inherits> (e[L"base"], e, dynamic_cast<Complex&> (scope ()));

    push (e);

    while (more ())
    {
      XML::Element e (next ());

      string name (e.name ());

      if (name == L"attribute") attribute (e); else
      {
        wcerr << "expected `attribute' instead of " << name << endl;
      }
    }

    pop ();
  }

  void Parser::
  complex_content_extension (XML::Element const& e)
  {
    if (trace_) wcout << "extension base: " << fq_name (e, e[L"base"]) << endl;

    set_type<Inherits> (e[L"base"], e, dynamic_cast<Complex&> (scope ()));

    push (e);

    while (more ())
    {
      XML::Element e (next ());

      string name (e.name ());

      if (name == L"group") group (e); else
      if (name == L"all") all (e); else
      if (name == L"choice") choice (e); else
      if (name == L"sequence") sequence (e); else
      if (name == L"attribute") attribute (e); else
      {
        wcerr << "expected `attribute' instead of " << name << endl;
      }
    }

    pop ();
  }

  void Parser::
  element (XML::Element const& e, bool global)
  {
    unsigned long min (e[L"minOccurs"] == L"0" ? 0 : this->min ()),
      max (e[L"maxOccurs"] && e[L"maxOccurs"] != L"1" ? 0 : this->max ());

    bool qualified (global ? true : qualify_element_);

    if (string form = e[L"form"])
    {
      qualified = form == L"qualified";
    }

    if (trace_) wcout << "element min " << min << " max " << max
                      << " qualified " << qualified << endl;

    if (string name = e[L"name"])
    {
      Element& node (root_schema_->new_node<Element> (min, max, qualified));
      root_schema_->new_edge<Names> (scope (), node, name);


      if (string type = e[L"type"])
      {
        if (trace_) wcout << "element type " << XML::fq_name (e, type) << endl;

        set_type<Belongs> (type, e, node);
      }
      else
      {
        // Looks like an anonymous type.
        //
        push (e);

        annotation ();

        if (more ())
        {
          XML::Element e (next ());

          string name (e.name ());

          if (trace_) wcout << name << endl;

          Type* t (0);

          if (name == L"simpleType")  t = simple_type (e); else
            if (name == L"complexType") t = complex_type (e); else
            {
              wcerr << "expected `simpleType' or `complexType' instead of "
                    << e.name () << endl;
            }

          if (t)
          {
            root_schema_->new_edge<Belongs> (node, *t);
          }
        }
        else
        {
          // anyType almighty.
          //
          string prefix (ns_prefix (e, xsd, true));
          type =  prefix + (prefix.empty () ? L"" : L":") + L"anyType";

          set_type<Belongs> (type, e, node);
        }

        pop ();
      }
    }
    else if (string ref = e[L"ref"])
    {
      string uq_name (XML::uq_name (ref));
      string ns_name (XML::ns_name (e, ref));

      Element& node (root_schema_->new_node<Element> (min, max, qualified));
      root_schema_->new_edge<Names> (scope (), node, uq_name);

      try
      {
        Element& e (resolve<Element> (ns_name, uq_name, *root_schema_));

        if (e.typed ())
        {
          root_schema_->new_edge<Belongs> (node, e.type ());
        }
        else if (e.context ().count ("type-ns-name"))
        {
          string ns_name (e.context ().get<string> ("type-ns-name"));
          string uq_name (e.context ().get<string> ("type-uq-name"));

          e.context ().set ("type-ns-name", ns_name);
          e.context ().set ("type-uq-name", uq_name);

          if (trace_)
            wcout << "element `" << ref << "' is not typed`" << endl
                  << "deferring resolution until later" << endl;
        }
        // Global element cannot reference.
        //
        else
        {
          // What the heck?
          //
          if (trace_)
            wcout << "element `" << ref << "' is in unexpected condition"
                  << endl;
        }
      }
      catch (NotNamespace const&)
      {
        wcerr << "unable to resolve namespace `" << ns_name << "'" << endl;
      }
      catch (NotName const&)
      {
        node.context ().set ("instance-ns-name", ns_name);
        node.context ().set ("instance-uq-name", uq_name);

        if (trace_)
          wcout << "unable to resolve name `" << uq_name
                << "' inside namespace `" << ns_name << "'" << endl
                << "deferring resolution until later" << endl;
      }
    }
    else
    {
      wcerr << "`name' or `ref' attribute is missing for element declaration"
            << endl;
    }
  }

  void Parser::
  annotation ()
  {
    if (more ())
    {
      if (next ().name () != L"annotation") prev ();
    }
  }


  void Parser::
  attribute (XML::Element const& a, bool global)
  {
    string name (a[L"name"]);

    if (name.empty ())
    {
      wcerr << "`name' attribute is missing for attribute declaration"
            << endl;
      return;
    }

    bool optional (true);

   string use (a[L"use"]);

    if (use == L"prohibited") return;
    else if (use == L"required") optional = false;


    bool qualified (global ? true : qualify_attribute_);

    if (string form = a[L"form"])
    {
      qualified = form == L"qualified";
    }

    Attribute& node (root_schema_->new_node<Attribute> (optional, qualified));
    root_schema_->new_edge<Names> (scope (), node, name);

    string type (a[L"type"]);

    if (!type)
    {
      string prefix (ns_prefix (a, xsd, true));
      type =  prefix + (prefix.empty () ? L"" : L":") + L"anySimpleType";
    }

    if (trace_) wcout << "attribute type " << XML::fq_name (a, type) << endl;

    set_type<Belongs> (type, a, node);
  }


  template <typename Edge, typename Node>
  void Parser::
  set_type (string const& type, XML::Element const& e, Node& node)
  {
    string ns_name (XML::ns_name (e, type));
    string uq_name (XML::uq_name (type));

    try
    {
      Type& t (resolve<Type> (ns_name, uq_name, *root_schema_));

      root_schema_->template new_edge<Edge> (node, t);
    }
    catch (NotNamespace const&)
    {
      wcerr << "unable to resolve namespace `" << ns_name << "'" << endl;
    }
    catch (NotName const&)
    {
      node.context ().set ("type-ns-name", ns_name);
      node.context ().set ("type-uq-name", uq_name);

      if (trace_)
      {
        wcout << "unable to resolve name `" << uq_name
              << "' inside namespace `" << ns_name << "'" << endl
              << "deferring resolution until later" << endl;
      }
    }
  }

  // Xerces DOoM.
  //
  //
  std::wostream&
  operator<< (std::wostream& o, XMLCh const* str)
  {
    char* s (Xerces::XMLString::transcode (str));

    o << s;

    Xerces::XMLString::release (&s); // idiot?
    return o;
  }

  class ErrorHandler : public  Xerces::DOMErrorHandler
  {
  public:
    ErrorHandler ()
        : failed_ (false)
    {
    }

    virtual bool
    handleError (Xerces::DOMError const& e)
    {
      wcerr << e.getLocation()->getURI() << ':'
            << e.getLocation()->getLineNumber() << '.'
            << e.getLocation()->getColumnNumber();

      switch (e.getSeverity())
      {
      case Xerces::DOMError::DOM_SEVERITY_WARNING:
        {
          wcerr << " warning: ";
          break;
        }
      default:
        {
          wcerr << " error: ";
          failed_ = true;
          break;
        }
      }

      wcerr << e.getMessage() << endl;

      return true;
    }

    bool
    failed () const
    {
      return failed_;
    }

  private:
    bool failed_;
  };

  DOMDocument* Parser::
  dom (fs::path const& tu)
  {
    try
    {
      // Initialize Xerces runtime
      //
      XMLPlatformUtils::Initialize();

      // Instantiate the DOM parser.
      //
      XMLCh const gLS[] = {chLatin_L, chLatin_S, chNull };

      // Get an implementation of the Load-Store (LS) interface.
      //
      DOMImplementationLS* impl (
        static_cast<DOMImplementationLS*>(
          DOMImplementationRegistry::getDOMImplementation(gLS)));

      // Create a DOMBuilder.
      //
      DOMBuilder* parser (
        impl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0));

      // Discard comment nodes in the document.
      //
      parser->setFeature (XMLUni::fgDOMComments, false);

      // Disable datatype normalization. The XML 1.0 attribute value
      // normalization always occurs though.
      //
      parser->setFeature (XMLUni::fgDOMDatatypeNormalization, true);

      // Do not create EntityReference nodes in the DOM tree. No
      // EntityReference nodes will be created, only the nodes
      // Corresponding to their fully expanded substitution text will be
      // created.
      //
      parser->setFeature (XMLUni::fgDOMEntities, false);

      // Perform Namespace processing.
      //
      parser->setFeature (XMLUni::fgDOMNamespaces, true);

      // Perform Validation
      //
      //parser->setFeature (XMLUni::fgDOMValidation, true);

      // Do not include ignorable whitespace in the DOM tree.
      //
      parser->setFeature (XMLUni::fgDOMWhitespaceInElementContent, false);

      // Enable the parser's schema support.
      //
      parser->setFeature (XMLUni::fgXercesSchema, true);

      // Enable full schema constraint checking, including checking which
      // may be time-consuming or memory intensive. Currently, particle
      // unique attribution constraint checking and particle derivation
      // restriction checking are controlled by this option.
      //
      parser->setFeature (XMLUni::fgXercesSchemaFullChecking, true);

      // The parser will treat validation error as fatal and will exit.
      //
      parser->setFeature (XMLUni::fgXercesValidationErrorAsFatal, true);

      ErrorHandler eh;
      parser->setErrorHandler(&eh);

      std::string uri (tu.string ());

      parser->loadGrammar (uri.c_str (), Grammar::SchemaGrammarType);

      if (eh.failed ())
      {
        parser->release();
        return 0;
      }

      XSDDOMParser* xsd_parser (
        new (XMLPlatformUtils::fgMemoryManager) XSDDOMParser ());

      xsd_parser->setValidationScheme (XercesDOMParser::Val_Never);
      xsd_parser->setDoNamespaces (true);
      //xsd_parser->setUserEntityHandler(fEntityHandler);
      //xsd_parser->setUserErrorReporter(fErrorReporter);

      XMLCh* file_name (XMLString::transcode (uri.c_str ()));

      InputSource* in (new (XMLPlatformUtils::fgMemoryManager)
                       LocalFileInputSource (file_name));

      XMLString::release (&file_name);

      // I have no clue what this is for.
      // Put a janitor on the input source.
      //
      Janitor<InputSource> jan (in);

      xsd_parser->parse (*in);

      DOMDocument* doc (xsd_parser->getDocument());

      parser->release();

      return doc;
    }
    catch (Xerces::DOMException const& e)
    {
      wcerr << "caught DOMException: " << e.code << endl;
    }
    catch (Xerces::XMLException const& e)
    {
      wcerr << "caught XMLException" << endl;
    }

    return 0;
  }
}
