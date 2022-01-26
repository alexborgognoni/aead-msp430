#include <stdio.h>

#include "giftb128.h"

#include "key_schedule.h"

#define U32BIG(x)\
  ((((x) & 0x000000FF) << 24) | (((x) & 0x0000FF00) << 8) | \
    (((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24))

#define U8BIG(x, y)\
  (x)[0] = (y) >> 24;\
(x)[1] = ((y) >> 16) & 0xff;\
(x)[2] = ((y) >> 8) & 0xff;\
(x)[3] = (y) & 0xff;

// The round constants according to the fixsliced representation

const u32 rconst[40] = {
  0x10000008,
  0x80018000,
  0x54000002,
  0x01010181,
  0x8000001f,
  0x10888880,
  0x6001e000,
  0x51500002,
  0x03030180,
  0x8000002f,
  0x10088880,
  0x60016000,
  0x41500002,
  0x03030080,
  0x80000027,
  0x10008880,
  0x4001e000,
  0x11500002,
  0x03020180,
  0x8000002b,
  0x10080880,
  0x60014000,
  0x01400002,
  0x02020080,
  0x80000021,
  0x10000080,
  0x0001c000,
  0x51000002,
  0x03010180,
  0x8000002e,
  0x10088800,
  0x60012000,
  0x40500002,
  0x01030080,
  0x80000006,
  0x10008808,
  0xc001a000,
  0x14500002,
  0x01020181,
  0x8000001a
};

extern void gift128_perm_msp_fixslicing(uint32_t * S, uint32_t * rkey);
extern void precompute_rkeys_msp(u32 * rkey,
  const u8 * key);

void precompute_rkeys(u32 * rkey,
  const u8 * key) {
  int i;

  rkey[0] = U32BIG(((u32 * ) key)[3]);
  rkey[1] = U32BIG(((u32 * ) key)[1]);
  rkey[2] = U32BIG(((u32 * ) key)[2]);
  rkey[3] = U32BIG(((u32 * ) key)[0]);

  for (int i = 0; i < 16; i += 2) {
    rkey[i + 4] = rkey[i + 1];
    rkey[i + 5] = KEY_UPDATE(rkey[i]);
  }

  for (i = 0; i < 20; i += 10) {
    REARRANGE_RKEY_0(rkey[i]);
    REARRANGE_RKEY_0(rkey[i + 1]);
    REARRANGE_RKEY_1(rkey[i + 2]);
    REARRANGE_RKEY_1(rkey[i + 3]);
    REARRANGE_RKEY_2(rkey[i + 4]);
    REARRANGE_RKEY_2(rkey[i + 5]);
    REARRANGE_RKEY_3(rkey[i + 6]);
    REARRANGE_RKEY_3(rkey[i + 7]);
  }

  for (i = 20; i < 80; i += 10) {
    rkey[i] = rkey[i - 19];
    rkey[i + 1] = KEY_TRIPLE_UPDATE_0(rkey[i - 20]);
    rkey[i + 2] = KEY_DOUBLE_UPDATE_1(rkey[i - 17]);
    rkey[i + 3] = KEY_TRIPLE_UPDATE_1(rkey[i - 18]);
    rkey[i + 4] = KEY_DOUBLE_UPDATE_2(rkey[i - 15]);
    rkey[i + 5] = KEY_TRIPLE_UPDATE_2(rkey[i - 16]);
    rkey[i + 6] = KEY_DOUBLE_UPDATE_3(rkey[i - 13]);
    rkey[i + 7] = KEY_TRIPLE_UPDATE_3(rkey[i - 14]);
    rkey[i + 8] = KEY_DOUBLE_UPDATE_4(rkey[i - 11]);
    rkey[i + 9] = KEY_TRIPLE_UPDATE_4(rkey[i - 12]);
    SWAPMOVE(rkey[i], rkey[i], 0x00003333, 16);
    SWAPMOVE(rkey[i], rkey[i], 0x55554444, 1);
    SWAPMOVE(rkey[i + 1], rkey[i + 1], 0x55551100, 1);
  }
}

void giftb128(u8 * ctext, u8 * ptext, u32 * rkey) {
  u32 state[4];
  state[0] = U32BIG(((u32 * ) ptext)[0]);
  state[1] = U32BIG(((u32 * ) ptext)[1]);
  state[2] = U32BIG(((u32 * ) ptext)[2]);
  state[3] = U32BIG(((u32 * ) ptext)[3]);

  for (unsigned int i = 0; i < 80; i += 10) {
    QUINTUPLE_ROUND(state, rkey + i, rconst + i / 2);
  }

  U8BIG(ctext, state[0]);
  U8BIG(ctext + 4, state[1]);
  U8BIG(ctext + 8, state[2]);
  U8BIG(ctext + 12, state[3]);
}

void print_rkeys(u32 * rkey) {
  printf("\n");
  for (unsigned int i = 0; i < 80; i++) {
    printf("%lx\n", rkey[i]);
  }
  printf("\n");
}

void print_state(u32 * S) {
  printf("S0 %08lx, S1 %08lx, S2 %08lx, S3 %08lx\n", S[0], S[1], S[2], S[3]);
}

void precompute_rkeys_v2(uint32_t * rkey,
  const uint8_t * key) {
  int i;

  // classical initialization
  rkey[0] = U32BIG(((uint32_t * ) key)[3]);
  rkey[1] = U32BIG(((uint32_t * ) key)[1]);
  rkey[2] = U32BIG(((uint32_t * ) key)[2]);
  rkey[3] = U32BIG(((uint32_t * ) key)[0]);

  // classical key-schedule
  for (i = 4; i < 80; i += 2) {
    rkey[i] = rkey[i - 3];
    rkey[i + 1] = KEY_UPDATE(rkey[i - 4]);
  }

  // transposition to fixsliced representation
  for (i = 0; i < 80; i += 10) {
    REARRANGE_RKEY_0(rkey[i]);
    REARRANGE_RKEY_0(rkey[i + 1]);
    REARRANGE_RKEY_1(rkey[i + 2]);
    REARRANGE_RKEY_1(rkey[i + 3]);
    REARRANGE_RKEY_2(rkey[i + 4]);
    REARRANGE_RKEY_2(rkey[i + 5]);
    REARRANGE_RKEY_3(rkey[i + 6]);
    REARRANGE_RKEY_3(rkey[i + 7]);
  }
}

void test_state_update(void) {

  u32 key[4] = {
    0x67452301,
    0xefcdab89,
    0x98badcfe,
    0x10325476
  };
  u32 rkey[80];

  precompute_rkeys(rkey, (u8 * ) key);

  u32 S[4];
  u32 si = 0x48D159E0;

  for (unsigned int i = 0; i < 4; i++) {
    S[i] = si;
    si = si - 0x12345678;
  }

  printf("\n----------------- ORIGINAL STATE -----------------\n");

  print_state(S);

  printf("\n----------------- STATE C -----------------\n");

  for (int i = 0; i < 8; i++) {
    QUINTUPLE_ROUND(S, rkey + (i * 10), rconst + (i * 5));
  }

  print_state(S);

  printf("\n----------------- STATE MSP -----------------\n");

  si = 0x48D159E0;
  for (unsigned int i = 0; i < 4; i++) {
    S[i] = si;
    si = si - 0x12345678;
  }

  gift128_perm_msp_fixslicing(S, rkey);

  print_state(S);

}

void test_keyschedule(void) {
  u32 key[4] = {
    0x67452301,
    0xefcdab89,
    0x98badcfe,
    0x10325476
  };
  u32 rkey[80] = {
    0
  };

  precompute_rkeys_msp(rkey, (u8 * ) key);

  printf("\n----------------- MSP KEYS -----------------\n");
  for (int i = 0; i < 80; i++) {
    printf("i=%d, %08lx\n", i + 1, rkey[i]);
  }

  printf("\n");

  memset(rkey, 0, sizeof(rkey));

  precompute_rkeys(rkey, (u8 * ) key);
  printf("\n----------------- ORIGINAL KEYS -----------------\n");
  for (int i = 0; i < 80; i++) {
    printf("i=%d, %08lx\n", i + 1, rkey[i]);
  }
}

int main(void) {
//    test_state_update();
//    test_keyschedule();
  return 0;
}