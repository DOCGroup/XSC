eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
  & eval 'exec perl -S $0 $argv:q'
  if 0;

use strict;
use warnings;

use lib "$ENV{ACE_ROOT}/bin";
use PerlACE::TestTarget;

my $target = PerlACE::TestTarget::create_target (1)
  || die "Create target 1 failed\n";

my $xsc = "$ENV{XSC_ROOT}/bin/xsc";
unless (-x $xsc) {
  die "ERROR: Cannot find executable $xsc\n";
}

my $status = 0;
my $index = 0;
for my $mode ('--cxx-cpp03', '--cxx-cpp11') {
  my $suffix = ".reject" . $$ . "_" . $index;
  ++$index;
  my $header = "readonly$suffix.hpp";
  my $source = "readonly$suffix.cpp";
  my $process = $target->CreateProcess (
    $xsc, "--backend cxx --cxx-read-only $mode " .
      "--cxx-header-suffix $suffix.hpp --cxx-source-suffix $suffix.cpp " .
      "readonly.xsd");
  my $result = $process->SpawnWaitKill (
    $target->ProcessStartWaitInterval ());

  if ($result == -1) {
    print STDERR "ERROR: XSC $mode read-only rejection test timed out\n";
    $status = 1;
  }
  elsif ($result == 0) {
    print STDERR "ERROR: XSC accepted read-only mode with $mode\n";
    $status = 1;
  }
  else {
    print "XSC rejected read-only mode with $mode as expected\n";
  }

  if (-e $header || -e $source) {
    print STDERR "ERROR: XSC generated output for unsupported read-only $mode\n";
    unlink $header if -e $header;
    unlink $source if -e $source;
    $status = 1;
  }
}

my $compat_suffix = ".compat" . $$;
my $compat_header = "readonly$compat_suffix.hpp";
my $compat_source = "readonly$compat_suffix.cpp";
my $compat_process = $target->CreateProcess (
  $xsc, "--backend cxx --cxx-char-type char --cxx-cpp03 " .
    "--cxx-generate-traversal-types --cxx-header-suffix " .
    "$compat_suffix.hpp --cxx-source-suffix $compat_suffix.cpp readonly.xsd");
my $compat_result = $compat_process->SpawnWaitKill (
  $target->ProcessStartWaitInterval ());

if ($compat_result != 0) {
  print STDERR "ERROR: XSC failed to generate writable C++03 traversal code\n";
  $status = 1;
}
elsif (open my $source_file, '<', $compat_source) {
  local $/;
  my $source_text = <$source_file>;
  close $source_file;

  if ($source_text =~ /\bnullptr\b/) {
    print STDERR "ERROR: C++03 traversal output contains nullptr\n";
    $status = 1;
  }
}
else {
  print STDERR "ERROR: XSC did not create C++03 traversal output\n";
  $status = 1;
}

unlink $compat_header if -e $compat_header;
unlink $compat_source if -e $compat_source;

exit $status;
