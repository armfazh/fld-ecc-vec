#ifndef PALETA_BENCH_H
#define PALETA_BENCH_H

void bench_third_party(void);
void bench_fp25519(void);
void bench_ecc25519(void);
void bench_x25519(void);
void bench_ed25519(void);
void bench_hash25519(void);

void bench_fp448(void);
void bench_ecc448(void);
void bench_x448(void);
void bench_ed448(void);

#endif /* PALETA_BENCH_H */
