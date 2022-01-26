#include <stdint.h>

#include <inttypes.h>

#include <stdio.h>

#include <string.h>

#include "io430.h"

#define MAXROUNDS 12
#define NROWS 3
#define NCOLUMS 4
#define NLANES (NCOLUMS * NROWS)

#define ROTL32(a, offset)((((uint32_t) a) << ((offset) % 32)) ^ (((uint32_t) a) >> ((32 - (offset)) % 32)))
#define index(__x, __y)((((__y) % NROWS) * NCOLUMS) + ((__x) % NCOLUMS))

extern void xoodoo_perm_msp(uint16_t * state, uint16_t nr);

typedef uint32_t tXoodooLane;

static
const uint32_t RC[MAXROUNDS] = {
  0x00000058,
  0x00000038,
  0x000003C0,
  0x000000D0,
  0x00000120,
  0x00000014,
  0x00000060,
  0x0000002C,
  0x00000380,
  0x000000F0,
  0x000001A0,
  0x00000012
};

void print_state(uint32_t * a) {
  printf("a00 %08"
    PRIx32 ", a01 %08"
    PRIx32 ", a02 %08"
    PRIx32 ", a03 %08"
    PRIx32 "\n", a[0 + 0], a[0 + 1], a[0 + 2], a[0 + 3]);
  printf("a10 %08"
    PRIx32 ", a11 %08"
    PRIx32 ", a12 %08"
    PRIx32 ", a13 %08"
    PRIx32 "\n", a[4 + 0], a[4 + 1], a[4 + 2], a[4 + 3]);
  printf("a20 %08"
    PRIx32 ", a21 %08"
    PRIx32 ", a22 %08"
    PRIx32 ", a23 %08"
    PRIx32 "\n", a[8 + 0], a[8 + 1], a[8 + 2], a[8 + 3]);
}

static void xoodoo_perm_C(tXoodooLane * a, tXoodooLane rc) {
  unsigned int x, y;
  tXoodooLane b[NLANES];
  tXoodooLane p[NCOLUMS];
  tXoodooLane e[NCOLUMS];

  /* Theta: Column Parity Mixer */
  for (x = 0; x < NCOLUMS; ++x)
    p[x] = a[index(x, 0)] ^ a[index(x, 1)] ^ a[index(x, 2)];
  for (x = 0; x < NCOLUMS; ++x)
    e[x] = ROTL32(p[(x - 1) % 4], 5) ^ ROTL32(p[(x - 1) % 4], 14);
  for (x = 0; x < NCOLUMS; ++x)
    for (y = 0; y < NROWS; ++y)
      a[index(x, y)] ^= e[x];

  /* Rho-west: plane shift */
  for (x = 0; x < NCOLUMS; ++x) {
    b[index(x, 0)] = a[index(x, 0)];
    b[index(x, 1)] = a[index(x - 1, 1)];
    b[index(x, 2)] = ROTL32(a[index(x, 2)], 11);
  }
  memcpy(a, b, sizeof(b));

  /* Iota: round constant */
  a[0] ^= rc;

  /* Chi: non linear layer */
  for (x = 0; x < NCOLUMS; ++x)
    for (y = 0; y < NROWS; ++y)
      b[index(x, y)] = a[index(x, y)] ^ (~a[index(x, y + 1)] & a[index(x, y + 2)]);
  memcpy(a, b, sizeof(b));

  /* Rho-east: plane shift */
  for (x = 0; x < NCOLUMS; ++x) {
    b[index(x, 0)] = a[index(x, 0)];
    b[index(x, 1)] = ROTL32(a[index(x, 1)], 1);
    b[index(x, 2)] = ROTL32(a[index(x + 2, 2)], 8);
  }
  memcpy(a, b, sizeof(b));

}

static void xoodoo_perm_C_unrolled(tXoodooLane * a, tXoodooLane rc) {

  tXoodooLane b[NLANES];
  tXoodooLane p[NCOLUMS];
  tXoodooLane e[NCOLUMS];

  /* Theta: Column Parity Mixer */
  p[0] = a[0] ^ a[4] ^ a[8];
  p[1] = a[1] ^ a[5] ^ a[9];
  p[2] = a[2] ^ a[6] ^ a[10];
  p[3] = a[3] ^ a[7] ^ a[11];

  e[0] = ROTL32(p[3], 5) ^ ROTL32(p[3], 14);
  e[1] = ROTL32(p[0], 5) ^ ROTL32(p[0], 14);
  e[2] = ROTL32(p[1], 5) ^ ROTL32(p[1], 14);
  e[3] = ROTL32(p[2], 5) ^ ROTL32(p[2], 14);

  a[0] ^= e[0];
  a[4] ^= e[0];
  a[8] ^= e[0];
  a[1] ^= e[1];
  a[5] ^= e[1];
  a[9] ^= e[1];
  a[2] ^= e[2];
  a[6] ^= e[2];
  a[10] ^= e[2];
  a[3] ^= e[3];
  a[7] ^= e[3];
  a[11] ^= e[3];

  /* Rho-west: plane shift */
  b[0] = a[0];
  b[4] = a[7];
  b[8] = ROTL32(a[8], 11);
  b[1] = a[1];
  b[5] = a[4];
  b[9] = ROTL32(a[9], 11);
  b[2] = a[2];
  b[6] = a[5];
  b[10] = ROTL32(a[10], 11);
  b[3] = a[3];
  b[7] = a[6];
  b[11] = ROTL32(a[11], 11);
  memcpy(a, b, sizeof(b));

  /* Iota: round constant */
  a[0] ^= rc;

  /* Chi: non linear layer */
  b[0] = a[0] ^ (~a[4] & a[8]); // x = 0, y = 0
  b[4] = a[4] ^ (~a[8] & a[0]); // x = 0, y = 1
  b[8] = a[8] ^ (~a[0] & a[4]); // x = 0, y = 2
  b[1] = a[1] ^ (~a[5] & a[9]); // x = 1, y = 0
  b[5] = a[5] ^ (~a[9] & a[1]); // x = 1, y = 1
  b[9] = a[9] ^ (~a[1] & a[5]); // x = 1, y = 2
  b[2] = a[2] ^ (~a[6] & a[10]); // x = 2, y = 0
  b[6] = a[6] ^ (~a[10] & a[2]); // x = 2, y = 1
  b[10] = a[10] ^ (~a[2] & a[6]); // x = 2, y = 2
  b[3] = a[3] ^ (~a[7] & a[11]); // x = 3, y = 0
  b[7] = a[7] ^ (~a[11] & a[3]); // x = 3, y = 1
  b[11] = a[11] ^ (~a[3] & a[7]); // x = 3, y = 2
  memcpy(a, b, sizeof(b));

  /* Rho-east: plane shift */
  b[0] = a[0];
  b[4] = ROTL32(a[4], 1);
  b[8] = ROTL32(a[10], 8);
  b[index(1, 0)] = a[1];
  b[index(1, 1)] = ROTL32(a[5], 1);
  b[index(1, 2)] = ROTL32(a[11], 8);
  b[2] = a[2];
  b[6] = ROTL32(a[6], 1);
  b[10] = ROTL32(a[8], 8);
  b[3] = a[3];
  b[7] = ROTL32(a[7], 1);
  b[11] = ROTL32(a[9], 8);
  memcpy(a, b, sizeof(b));
}

void Xoodoo_Permute_Nrounds(uint32_t * a, tXoodooLane * RC, uint32_t nr) {
  unsigned int i;

  for (i = MAXROUNDS - nr; i < MAXROUNDS; ++i) {
    xoodoo_perm_C(a, RC[i]);
  }
}

static void xoodoo_theta_C(uint32_t * a) {

  unsigned int x, y;
  uint32_t b[NLANES];
  uint32_t p[NCOLUMS];
  uint32_t e[NCOLUMS];

  for (x = 0; x < NCOLUMS; ++x)
    p[x] = a[index(x, 0)] ^ a[index(x, 1)] ^ a[index(x, 2)];
  for (x = 0; x < NCOLUMS; ++x)
    e[x] = ROTL32(p[(x - 1) % 4], 5) ^ ROTL32(p[(x - 1) % 4], 14);
  for (x = 0; x < NCOLUMS; ++x)
    for (y = 0; y < NROWS; ++y)
      a[index(x, y)] ^= e[x];
}

static void xoodoo_rhowest_C(uint32_t * a) {

  unsigned int x;
  uint32_t b[NLANES];

  for (x = 0; x < NCOLUMS; ++x) {
    b[index(x, 0)] = a[index(x, 0)];
    b[index(x, 1)] = a[index(x - 1, 1)];
    b[index(x, 2)] = ROTL32(a[index(x, 2)], 11);
  }
  memcpy(a, b, sizeof(b));
}

static void xoodoo_iota_C(uint32_t * a, uint32_t rc) {

  a[0] ^= rc;

}

static void xoodoo_chi_C(uint32_t * a) {

  unsigned int x, y;
  uint32_t b[NLANES];

  for (x = 0; x < NCOLUMS; ++x)
    for (y = 0; y < NROWS; ++y)
      b[index(x, y)] = a[index(x, y)] ^ (~a[index(x, y + 1)] & a[index(x, y + 2)]);
  memcpy(a, b, sizeof(b));
}

static void xoodoo_rhoeast_C(uint32_t * a) {

  unsigned int x;
  uint32_t b[NLANES];

  for (x = 0; x < NCOLUMS; ++x) {
    b[index(x, 0)] = a[index(x, 0)];
    b[index(x, 1)] = ROTL32(a[index(x, 1)], 1);
    b[index(x, 2)] = ROTL32(a[index(x + 2, 2)], 8);
  }
  memcpy(a, b, sizeof(b));
}

void test_state_update(void) {
  uint16_t nr = 1;
  uint32_t a[12];
  uint32_t ai = 0xc83fb728;

  for (int i = 0; i < 12; i++) {
    a[i] = ai;
    ai = ai - 0x12345678;
  }

  printf("\n------------------ ORIGINAL STATE ------------------\n");
  print_state(a);

  printf("\n---------------- STATE AFTER C ----------------\n");

  unsigned int i;

  for (i = 0; i < nr; i++) {
    xoodoo_iota_C(a, RC[i]);
  }

  print_state(a);

  ai = 0xc83fb728;
  for (int i = 0; i < 12; i++) {
    a[i] = ai;
    ai = ai - 0x12345678;
  }

  printf("\n---------------- STATE AFTER MSP ----------------\n");
  xoodoo_perm_msp((uint16_t * ) a, nr);
  print_state(a);
}

int main(void) {

//  test_state_update();

  return 0;
}