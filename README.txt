==========================================================
  High Performance Implementation of the Edwards Digital
       Signature Algorithm using Vector Instructions     
==========================================================

To compile use:
 $ mkdir build
 $ cd build
 $ cmake ..
 $ make

For running the benchmark program use:
 $ bin/bench

For running the tests program use:
 $ bin/tests

In order to obtain the fastest timings
reported in paper, consider to use the
GCC compiler version 4.8.5.
then use your GCC 4.8.5 as:
 $ mkdir build
 $ cd build
 $ export CC=/path/to/gcc4.8.5
 $ cmake ..
 $ make

==================================================
