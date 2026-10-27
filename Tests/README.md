# XSC tests

The generated-code tests are compiled through `Tests.mwc`. After building XSC
and its ACE dependencies, run the full test suite locally with:

```sh
source ./setenv.sh
export LD_LIBRARY_PATH="$XSC_ROOT/XSC/utils:$XSC_ROOT/lib:$ACE_ROOT/lib:$LD_LIBRARY_PATH"
perl "$ACE_ROOT/bin/mwc.pl" -type gnuace Tests/Tests.mwc -workers 4
make -j8 -C Tests
perl "$ACE_ROOT/bin/auto_run_tests.pl" -r "$XSC_ROOT" -l Tests/run_test.lst
```

`run_test.lst` lists the runtime test drivers. The other test directories contain
compile-time generated-code and API checks, which run as part of the MPC build.
