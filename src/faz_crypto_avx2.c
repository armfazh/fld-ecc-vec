/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of eddsa_avx2.
 *
 * eddsa_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * eddsa_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with eddsa_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 * \mainpage eddsa_avx2 - Project Documentation

eddsa_avx2 is an optimized and secure software library for computing Edwards Digital Signature Algorithm (EdDSA) and the Diffie-Hellman functions X25519 and X448. This library was specially accelerated with the Intel Advanced Vector eXtensions version 2 (AVX2).

This library belongs to the research project titled:
> High Performance Implementation of the Edwards Digital Signature Algorithm using Vector Instructions
which is authored by:
 - Armando Faz Hernández (armfazh@ic.unicamp.br)
 - Julio López (jlopez@ic.unicamp.br)
 - Ricardo Dahab (rdahab@ic.unicamp.br)

This research work is part of a paper submission to the ACM Journal of Mathematical Software.
Software will be available after paper acceptance. During this reviewing process, source code is
available to reviewers and journal editors through the submission website.

Compilation
===========

To compile use:
~~~~
    $ cd eddsa_avx2
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
    $ cd eddsa_avx2
    $ mkdir build; cd build
    $ export CC=/path/to/gcc4.8.5
    $ cmake ..
    $ make
~~~~

Getting started
===============

 - Prime field arithmetic [\link faz_fp_avx2.h faz_fp_avx2.h \endlink ]
 - Elliptic curve Diffie-Hellman [\link faz_ecdh_avx2.h faz_ecdh_avx2.h \endlink ]
 - Edwards Digital Signature Algorithm [\link faz_eddsa_avx2.h faz_eddsa_avx2.h \endlink ]


License
=======
This software library is released under the term provided by GNU Lesser General Public License v3.0.
 *
 **/

#include "faz_fp_avx2.c"
#include "faz_eddsa_avx2.c"
#include "faz_ecdh_avx2.c"


#ifdef __cplusplus
};
#endif /* __cplusplus */
