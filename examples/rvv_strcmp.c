#include "common.h"
#include <riscv_vector.h>
#include <string.h>

// reference https://github.com/riscv/riscv-v-spec/blob/master/example/strcmp.s
int strcmp_vec(const char *src1, const char *src2) {
  const unsigned char *s1 = (const void*)src1, *s2 = (const void*)src2;
  size_t vlmax = __riscv_vsetvlmax_e8m2();
  long first_set_bit = -1;
  size_t vl;
  for (; first_set_bit < 0; s1 += vl, s2 += vl) {
    vuint8m2_t vec_src1 = __riscv_vle8ff_v_u8m2(s1, &vl, vlmax);
    vuint8m2_t vec_src2 = __riscv_vle8ff_v_u8m2(s2, &vl, vl);

    vbool4_t string_terminate = __riscv_vmseq_vx_u8m2_b4(vec_src1, 0, vl);
    vbool4_t no_equal = __riscv_vmsne_vv_u8m2_b4(vec_src1, vec_src2, vl);
    vbool4_t vec_terminate = __riscv_vmor_mm_b4(string_terminate, no_equal, vl);

    first_set_bit = __riscv_vfirst_m_b4(vec_terminate, vl);
  }
  s1 -= vl - first_set_bit;
  s2 -= vl - first_set_bit;
  return *s1 - *s2;
}

int main() {
  const int N = 1023;
  const uint32_t seed = 0xdeadbeef;
  srand(seed);

  // data gen
  char s0[N], s1[N];
  gen_string(s0, N);
  gen_string(s1, N);

  // compute
  int golden, actual;
  golden = strcmp(s0, s1);
  actual = strcmp_vec(s0, s1);

  // compare
  puts(golden == actual ? "pass" : "fail");
}
