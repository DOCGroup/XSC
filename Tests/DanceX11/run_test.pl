eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
  & eval 'exec perl -S $0 $argv:q'
  if 0;

use strict;
use warnings;

use lib "$ENV{ACE_ROOT}/bin";
use PerlACE::TestTarget;

my $status = 0;
my $target = PerlACE::TestTarget::create_target (1)
  || die "Create target 1 failed\n";

my $test = "$ENV{XSC_ROOT}/Tests/DanceX11/optional_copy";
unless (-x $test) {
  die "ERROR: Cannot find executable $test\n";
}

my $process = $target->CreateProcess ($test);
my $result = $process->SpawnWaitKill ($target->ProcessStartWaitInterval ());

if ($result == -1) {
  print STDERR "ERROR: DanceX11 optional-copy test could not be started or timed out\n";
  $status = 1;
}
elsif ($result != 0) {
  print STDERR "ERROR: generated copy assignment did not clear the optional href\n";
  $status = 1;
}
else {
  print "DanceX11 generated copy assignment passed\n";
}

my $required_members_test = "$ENV{XSC_ROOT}/Tests/DanceX11/required_members";
unless (-x $required_members_test) {
  die "ERROR: Cannot find executable $required_members_test\n";
}

$target->AddLibPath ("$ENV{XSC_ROOT}/Tests/DanceX11");
my $required_members_process = $target->CreateProcess ($required_members_test);
my $required_members_result = $required_members_process->SpawnWaitKill (
  $target->ProcessStartWaitInterval ());

if ($required_members_result == -1) {
  print STDERR "ERROR: DanceX11 required-member test could not be started or timed out\n";
  $status = 1;
}
elsif ($required_members_result != 0) {
  print STDERR "ERROR: read-only parser accepted a missing required child element\n";
  $status = 1;
}
else {
  print "DanceX11 generated required-member checks passed\n";
}

exit $status;
