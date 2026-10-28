# XSC tests

The generated-code tests are compiled through `Tests.mwc`. From the XSC repository
root, set the environment variables for your ACE/TAO and MPC checkouts, then run
the full test suite after building XSC and its ACE dependencies:

```sh
export XSC_ROOT="$(pwd)"
export DOC_ROOT="/path/to/ACE_TAO"
export ACE_ROOT="$DOC_ROOT/ACE"
export TAO_ROOT="$DOC_ROOT/TAO"
export MPC_ROOT="/path/to/MPC"
export LD_LIBRARY_PATH="$XSC_ROOT/XSC/utils:$XSC_ROOT/lib:$ACE_ROOT/lib:$LD_LIBRARY_PATH"
perl "$ACE_ROOT/bin/mwc.pl" -type gnuace Tests/Tests.mwc -workers 4
make -j8 -C Tests
perl "$ACE_ROOT/bin/auto_run_tests.pl" -r "$XSC_ROOT" -l Tests/run_test.lst
```

`run_test.lst` lists the runtime test drivers. The other test directories contain
compile-time generated-code and API checks, which run as part of the MPC build.
