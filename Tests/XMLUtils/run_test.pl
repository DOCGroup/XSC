eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
  & eval 'exec perl -S $0 $argv:q'
  if 0;

use strict;
use warnings;

use lib "$ENV{ACE_ROOT}/bin";
use PerlACE::TestTarget;

my $target = PerlACE::TestTarget::create_target (1)
  || die "Create target 1 failed\n";

my $test = "$ENV{XSC_ROOT}/Tests/XMLUtils/runtime";
unless (-x $test) {
  die "ERROR: Cannot find executable $test\n";
}

my $process = $target->CreateProcess ($test);
my $result = $process->SpawnWaitKill ($target->ProcessStartWaitInterval ());

if ($result == -1) {
  print STDERR "ERROR: ACE XML_Utils runtime regression timed out\n";
  exit 1;
}
elsif ($result != 0) {
  print STDERR "ERROR: ACE XML_Utils runtime regression failed with status $result\n";
  exit 1;
}

print "ACE XML_Utils runtime regression passed\n";
exit 0;
