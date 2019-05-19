# FLD ECC AVX2

This is a mathematical software library for computing the Edwards Digital Signature Algorithm (EdDSA) and the Diffie-Hellman functions X25519 and X448. The library is optimized with the Intel Advanced Vector eXtensions version 2 (AVX2) and is derived from a research project.

```bibtex
@article{fld_ecc_avx2,
    author    = {A. Faz-Hern\'{a}ndez and J. L\'{o}pez and 
                 Ricardo Dahab},
    title     = {High Performance Implementation of Elliptic Curve
                 Cryptography using Vector Instructions},
    year      = {2019},
    journal   = {ACM Transactions on Mathematical Software (TOMS)},
    publisher = {ACM},
    volume    = {45},
    number    = {3},
    keywords  = {Edwards Digital Signature Algorithm, Ed25519,
                 Diffie-Hellman Protocol, AVX2 Vector Instructions, 
                 Elliptic Curve Cryptography, Secure Software},
    note      = {To appear},
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
    $ ./bin/tests
```

### LICENSE

BSD 3-Clause [LICENSE.txt](LICENSE.txt)

