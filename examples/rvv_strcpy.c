#include "common.h"
#include <assert.h>
#include <riscv_vector.h>
#include <string.h>

// reference https://github.com/riscv/riscv-v-spec/blob/master/example/strcpy.s
char *strcpy_vec(char *dst, const char *src) {
  unsigned char *d = (unsigned char*)dst;
  unsigned char *s = (unsigned char*)src;
  size_t vlmax = __riscv_vsetvlmax_e8m8();
  long first_set_bit = -1;
  for (size_t vl; first_set_bit < 0; s += vl, d += vl) {
    vuint8m8_t vec_src = __riscv_vle8ff_v_u8m8(s, &vl, vlmax);

    vbool1_t string_terminate = __riscv_vmseq_vx_u8m8_b1(vec_src, 0, vl);
    vbool1_t mask = __riscv_vmsif_m_b1(string_terminate, vl);

    __riscv_vse8_v_u8m8_m(mask, d, vec_src, vl);
    first_set_bit = __riscv_vfirst_m_b1(string_terminate, vl);
  }
  return dsr;
}

int main() {
  const int N = 2000;
  const uint32_t seed = 0xdeadbeef;
  srand(seed);

  // data gen
  char s0[N];
  gen_string(s0, N);

  // compute
  char golden[N], actual[N];
  strcpy(golden, s0);
  strcpy_vec(actual, s0);

  // compare
  puts(strcmp(golden, actual) == 0 ? "pass" : "fail");
}
