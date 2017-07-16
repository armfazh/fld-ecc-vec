faz_ecc_avx2 - Project Documentation
====================================

faz_ecc_avx2 is an optimized mathematical software library for computing
Edwards Digital Signature Algorithm (EdDSA) and the Diffie-Hellman 
functions X25519 and X448. This library was specially accelerated with
the Intel Advanced Vector eXtensions version 2 (AVX2). This library 
belongs to the research project titled:
> “High Performance Implementation of Elliptic Curve
       Cryptography using Vector Instructions”

authored by:
• Armando Faz Hernández, University of Campinas, Brazil.
 (armfazh@ic.unicamp.br)
• Julio López, University of Campinas, Brazil.
• Ricardo Dahab, University of Campinas, Brazil.

This research work is part of a paper submission to the ACM Journal of
Mathematical Software. Software will be publicly available after paper
acceptance in the Collected Algorithms from ACM (CALGO) website and in
the ACM Digital Library. During reviewing process, source code is 
available only to reviewers and journal editors through the ACM 
submission website.


Compilation
===========

To compile use:
~~~~
    $ unzip faz_ecc_avx2.zip
    $ cd faz_ecc_avx2
    $ mkdir build; cd build
    $ cmake ..
    $ make all
~~~~

For running the benchmark program use:
~~~~
    $ ./bin/bench
~~~~

For running the tests program use:
~~~~
    $ ./bin/tests
~~~~

In order to obtain the fastest timings
reported in paper, consider to use the
GCC compiler version 4.8.5.
then use your GCC 4.8.5 as:
~~~~
    $ cd faz_ecc_avx2
    $ mkdir build; cd build
    $ export CC=/path/to/gcc4.8.5
    $ cmake ..
    $ make
~~~~

License
=======
This software library is released under the term provided
by GNU Lesser General Public License v3.0.


