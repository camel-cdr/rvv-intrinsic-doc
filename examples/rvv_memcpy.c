#include "common.h"
#include <riscv_vector.h>
#include <string.h>

void *memcpy_vec(void *restrict dst, const void *restrict src, size_t n) {
  unsigned char *d = dst;
  const unsigned char *s = src;
  // copy data byte by byte
  for (size_t vl; n > 0; n -= vl, s += vl, d += vl) {
    vl = __riscv_vsetvl_e8m8(n);
    vuint8m8_t vec_src = __riscv_vle8_v_u8m8(s, vl);
    __riscv_vse8_v_u8m8(d, vec_src, vl);
  }
  return dst;
}

int main() {
  const int N = 127;
  const uint32_t seed = 0xdeadbeef;
  srand(seed);

  // data gen
  double A[N];
  gen_rand_1d(A, N);

  // compute
  double golden[N], actual[N];
  memcpy(golden, A, sizeof(A));
  memcpy_vec(actual, A, sizeof(A));

  // compare
  puts(compare_1d(golden, actual, N) ? "pass" : "fail");
}
