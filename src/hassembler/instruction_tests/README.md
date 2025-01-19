## How to build tests

The repo doesn't contain the files with the test results, therefore they must
be generated from a stable build.

To generate the results in Windows:

```batch
cd instruction_tests
build all
genresults
```
To run tests in Windows:

```batch
cd instruction_tests
build all
test
```