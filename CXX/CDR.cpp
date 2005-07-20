// file      : CXX/CDR.cpp
// author    : Arvind S. Krishna <arvindkr@qualcomm.com>
// cvs-id    : $Id$ 

#include <CXX/Forward.hpp>
#include <CXX/Elements.hpp>

#include <XSC/SemanticGraph.hpp>
#include <XSC/Traversal.hpp>

namespace
{
  struct Complex : Traversal::Complex, protected virtual Context
  {
    Complex (Context& c)
        : Context (c)
    {
    }

    virtual void
    traverse (Type& e)
    {
      // check if reader/writer flags are set in the 
      // context
      if (this->cdr_reader_generation_enabled ())
	os << "extern operator >>" << endl
	   << "(::XMLSchema::CDR_OutputStream &," << endl
	   << " ::XMLSchema::cdr_arg_traits < " 
	   << e.name () << " >::inout_type " << endl
	   << ");" << endl;

      if (this->cdr_writer_generation_enabled ())
	




void
generate_cdr_operators (Context& ctx, 
			SemanticGraph::Schema& schema)
{
  ctx.os << "// istream/ostream declarations." << endl
         << "//" << endl;

  Traversal::Schema traverser;
  Traversal::Sources sources;
  Traversal::Names schema_names;
  Namespace ns (ctx);

  traverser.edge_traverser (sources);
  traverser.edge_traverser (schema_names);
  sources.node_traverser (traverser);
  schema_names.node_traverser (ns);

  Traversal::Names names;
  Complex complex (ctx);
  Enumeration enumeration (ctx);

  ns.edge_traverser (names);
  names.node_traverser (complex);
  names.node_traverser (enumeration);

  traverser.traverse (schema);
}
