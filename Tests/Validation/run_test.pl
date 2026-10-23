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

if ($result == -1) {
  print STDERR "ERROR: XSC could not be started or timed out\n";
  $status = 1;
}
elsif ($result == 0) {
  print STDERR "ERROR: XSC accepted test.xsd; rejection was expected\n";
  $status = 1;
}
else {
  print "XSC rejected test.xsd as expected\n";
}

my $banner = "banner_$$\xFF.txt";
my $schema = "$ENV{XSC_ROOT}/Tests/Abstract/abstract.xsd";
open my $banner_file, '>', $banner
  or die "ERROR: Cannot create non-ASCII banner file $banner\n";
close $banner_file;

for my $backend (
  ['cxx', '--cxx-banner-file', "--cxx-header-suffix .args.$$.hpp --cxx-source-suffix .args.$$.cpp", "abstract.args.$$.hpp"],
  ['idl', '--idl-banner-file', "--idl-file-suffix .args.$$.idl", "abstract.args.$$.idl"])
{
  my ($name, $banner_option, $suffix_options, $output) = @$backend;
  my $process = $target->CreateProcess (
    $xsc, "--backend $name $banner_option $banner $suffix_options $schema");
  my $result = $process->SpawnWaitKill ($target->ProcessStartWaitInterval ());

  if ($result != 0) {
    print STDERR "ERROR: XSC $name generation with a non-ASCII argument failed\n";
    $status = 1;
  }
  elsif (open my $generated, '<', $output) {
    binmode $generated;
    local $/;
    my $text = <$generated>;
    close $generated;
    my ($arguments) = $text =~ m{^// Arguments: ([^\r\n]*)}m;
    if (!defined $arguments || index ($arguments, "\xFF") != -1 ||
        index ($arguments, '?') == -1) {
      print STDERR "ERROR: XSC $name did not safely record non-ASCII arguments\n";
      $status = 1;
    }
  }
  else {
    print STDERR "ERROR: XSC did not create $output\n";
    $status = 1;
  }

  unlink $output if -e $output;
  if ($name eq 'cxx') {
    (my $source = $output) =~ s/\.hpp$/.cpp/;
    unlink $source if -e $source;
  }
}

unlink $banner;

exit $status;
