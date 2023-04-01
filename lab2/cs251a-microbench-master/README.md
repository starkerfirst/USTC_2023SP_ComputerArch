Simple Example Microbenchmarks

A few simple microbenchmarks for you to test.

Note that some of the benchmarks do something useful, but others do not.  The
beauty of microbenchmarks is that they don't have to be meaningful, just evoke
interesting behavior in the microarchitecture.

# Input Generation

Two of the benchmarks requires a random array (randArr.h and spmvArr.h) to be used as an input.  Run this command to run the generation scripts.  (you can also look at the python scripts to generate your own kind of arrays)

```
make gen_arr
```

# Build Instructions:

Make all the benchmarks with the following command:

```
make
```


