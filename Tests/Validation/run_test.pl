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

my $xsc = "$ENV{XSC_ROOT}/bin/xsc";

unless (-x $xsc) {
  die "ERROR: Cannot find executable $xsc\n";
}

my $process = $target->CreateProcess ($xsc, '--backend cxx test.xsd');
my $result = $process->SpawnWaitKill ($target->ProcessStartWaitInterval ());

if ($result == 0) {
  print STDERR "ERROR: XSC accepted test.xsd; rejection was expected\n";
  $status = 1;
}
else {
  print "XSC rejected test.xsd as expected\n";
}

exit $status;
