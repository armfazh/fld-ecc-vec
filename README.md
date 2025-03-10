# FLD ECC AVX2/AVX512

This is a mathematical software library for computing the Edwards Digital Signature Algorithm (EdDSA) and the Diffie-Hellman functions X25519 and X448. The library is optimized with the Intel Advanced Vector eXtensions AVX2 and AVX512 product of this research project.

```bibtex
@article{fld_ecc_avx2,
    doi       = {10.1145/3309759},
    author    = {Armando Faz-Hern\'{a}ndez and
                 Julio L\'{o}pez and
                 Ricardo Dahab},
    title     = {High-performance Implementation of Elliptic Curve
                 Cryptography Using Vector Instructions},
    year      = {2019},
    journal   = {ACM Transactions on Mathematical Software (TOMS)},
    publisher = {ACM},
    volume    = {45},
    number    = {3},
    keywords  = {Edwards Digital Signature Algorithm, Ed25519,
                 Diffie-Hellman Protocol, AVX2 Vector Instructions,
                 Elliptic Curve Cryptography, Secure Software},
}
```

### Compilation

To compile use:
```bash
    $ cd fld_ecc_avx2
    $ mkdir build; cd build
    $ cmake ..
    $ make all
```

#### Enabling AVX512

Use AVX512 extensions to perform two X25519 operations simultaneously.
```bash
    $ cmake .. -DENABLE_AVX512=ON
    $ make all
```

### Benchmarks

For running the benchmark program use:

```bash
    $ ./bin/bench
```

The timings reported in the paper were obtained using the Clang compiler version 5.

```bash
    $ cd fld_ecc_avx2
    $ mkdir build; cd build
    $ CC=/path/to/clang cmake ..
    $ make
```

### Tests

For running the tests program use:
```bash
    $ make test_mine unit_tests
    $ ./bin/test_mine
    $ ./bin/unit_tests
```

### Formating Code

For formating code use [astyle](https://astyle.sourceforge.net/):
```bash
    $ astyle --style=linux ./src/*c ./include/*.h
```

### LICENSE

BSD 3-Clause [LICENSE.txt](LICENSE.txt)
