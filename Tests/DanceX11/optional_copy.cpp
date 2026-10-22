#include "Basic_Deployment_Data.hpp"

int
main ()
{
  Deployment::DataValue sequence;
  Deployment::DataValue sequence_copy (sequence);
  sequence_copy = sequence;

  Deployment::IdRef destination;
  XMLSchema::string<char> href ("urn:destination");
  destination.href (href);

  Deployment::IdRef source;
  destination = source;

  return destination.href_p () ? 1 : 0;
}
