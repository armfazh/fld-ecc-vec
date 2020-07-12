/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of faz_ecc_avx2.
 *
 * faz_ecc_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * faz_ecc_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with faz_ecc_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * \mainpage faz_ecc_avx2 - Project Documentation

faz_ecc_avx2 is an optimized and secure software library for computing Edwards
Digital Signature Algorithm (EdDSA) and the Diffie-Hellman functions X25519 and
X448. This library was specially accelerated with the Intel Advanced Vector
eXtensions version 2 (AVX2).

This library belongs to the research project titled:
> High Performance Implementation of Elliptic Curve Cryptography using Vector
Instructions which is authored by:
 - Armando Faz Hernández (armfazh@ic.unicamp.br), University of Campinas,
Brazil.
 - Julio López, University of Campinas, Brazil.
 - Ricardo Dahab, University of Campinas, Brazil.

This research work is part of a paper submission to the ACM Journal of
Mathematical Software.

Compilation
===========

To compile use:
~~~~
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
Clang compiler.
~~~~
    $ cd faz_ecc_avx2
    $ mkdir build; cd build
    $ export CC=/path/to/clang
    $ cmake ..
    $ make
~~~~

Getting started
===============

 - Prime field arithmetic [\link faz_fp_avx2.h faz_fp_avx2.h \endlink]
 - Elliptic curve Diffie-Hellman [\link faz_ecdh_avx2.h faz_ecdh_avx2.h
\endlink]
 - Edwards Digital Signature Algorithm [\link faz_eddsa_avx2.h faz_eddsa_avx2.h
\endlink]

LICENSE
===============
See the license file [\link ../LICENSE.txt LICENSE.txt \endlink ]

 *
 **/

#define SCOPE static inline

#define BYTESTRING_C 1
#include "bytestring.c"

#define FP255 1
#include "fp255.c"
#define ED255 1
#include "ed255.c"
#define DH255 1
#include "dh255.c"
#define HASH255 1
#include "hash255.c"

#define FP448 1
#include "fp448.c"
#define ED448 1
#include "ed448.c"
#define DH448 1
#include "dh448.c"
