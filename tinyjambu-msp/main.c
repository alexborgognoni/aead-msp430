#include <string.h>

#include <stdio.h>

#include <stdint.h>

#include "io430.h"

#define NROUND1 128 * 5
#define NROUND2 128 * 8

extern void state_update(uint16_t * state,
  const uint16_t * key, uint16_t number_of_steps);

void print_twords_16(int idx, uint16_t t1, uint16_t t2, uint16_t t3, uint16_t t4) {
  printf("\n----------------- State[%i] 16-bit modification -----------------\n", idx);
  printf("t1 : %04x\n", t1);
  printf("t2 : %04x\n", t2);
  printf("t3 : %04x\n", t3);
  printf("t4 : %04x\n", t4);
}

void print_state(uint32_t * state) {
  int i;

  printf("-------- 128-bit state: --------\n");
  for (i = 0; i < 4; i++) {
    printf("%08lx", state[i]);
  }
  printf("\n");
}

void state_update_16(uint16_t * state,
  const uint16_t * key, uint16_t number_of_steps) {

  uint16_t t1, t2;

  for (unsigned int i = 0; i < number_of_steps; i = i + 128) {
    t1 = (state[2] >> 15) | (state[3] << 1);
    t2 = (state[5] >> 11) | (state[6] << 5); // this was formerly t4
    state[0] ^= t1;
    state[0] ^= t2;
    t1 = (state[4] >> 6) | (state[5] << 10); // this was formerly t2
    t2 = (state[5] >> 5) | (state[6] << 11); // this was formerly t3
    t1 &= t2;
    t1 = ~t1;
    state[0] ^= t1;
    state[0] ^= key[0];

    t1 = (state[3] >> 15) | (state[4] << 1);
    t2 = (state[6] >> 11) | (state[7] << 5); // this was formerly t4
    state[1] ^= t1;
    state[1] ^= t2;
    t1 = (state[5] >> 6) | (state[6] << 10); // this was formerly t2
    t2 = (state[6] >> 5) | (state[7] << 11); // this was formerly t3
    t1 &= t2;
    t1 = ~t1;
    state[1] ^= t1;
    state[1] ^= key[1];

    t1 = (state[4] >> 15) | (state[5] << 1);
    t2 = (state[7] >> 11) | (state[0] << 5); // this was formerly t4
    state[2] ^= t1;
    state[2] ^= t2;
    t1 = (state[6] >> 6) | (state[7] << 10); // this was formerly t2
    t2 = (state[7] >> 5) | (state[0] << 11); // this was formerly t3
    t1 &= t2;
    t1 = ~t1;
    state[2] ^= t1;
    state[2] ^= key[2];

    t1 = (state[5] >> 15) | (state[6] << 1);
    t2 = (state[0] >> 11) | (state[1] << 5); // this was formerly t4
    state[3] ^= t1;
    state[3] ^= t2;
    t1 = (state[7] >> 6) | (state[0] << 10); // this was formerly t2
    t2 = (state[0] >> 5) | (state[1] << 11); // this was formerly t3
    t1 &= t2;
    t1 = ~t1;
    state[3] ^= t1;
    state[3] ^= key[3];

    t1 = (state[6] >> 15) | (state[7] << 1);
    t2 = (state[1] >> 11) | (state[2] << 5); // this was formerly t4
    state[4] ^= t1;
    state[4] ^= t2;
    t1 = (state[0] >> 6) | (state[1] << 10); // this was formerly t2
    t2 = (state[1] >> 5) | (state[2] << 11); // this was formerly t3
    t1 &= t2;
    t1 = ~t1;
    state[4] ^= t1;
    state[4] ^= key[4];

    t1 = (state[7] >> 15) | (state[0] << 1);
    t2 = (state[2] >> 11) | (state[3] << 5); // this was formerly t4
    state[5] ^= t1;
    state[5] ^= t2;
    t1 = (state[1] >> 6) | (state[2] << 10); // this was formerly t2
    t2 = (state[2] >> 5) | (state[3] << 11); // this was formerly t3
    t1 &= t2;
    t1 = ~t1;
    state[5] ^= t1;
    state[5] ^= key[5];

    t1 = (state[0] >> 15) | (state[1] << 1);
    t2 = (state[3] >> 11) | (state[4] << 5); // this was formerly t4
    state[6] ^= t1;
    state[6] ^= t2;
    t1 = (state[2] >> 6) | (state[3] << 10); // this was formerly t2
    t2 = (state[3] >> 5) | (state[4] << 11); // this was formerly t3
    t1 &= t2;
    t1 = ~t1;
    state[6] ^= t1;
    state[6] ^= key[6];

    t1 = (state[1] >> 15) | (state[2] << 1);
    t2 = (state[4] >> 11) | (state[5] << 5); // this was formerly t4
    state[7] ^= t1;
    state[7] ^= t2;
    t1 = (state[3] >> 6) | (state[4] << 10); // this was formerly t2
    t2 = (state[4] >> 5) | (state[5] << 11); // this was formerly t3
    t1 &= t2;
    t1 = ~t1;
    state[7] ^= t1;
    state[7] ^= key[7];
  }
}

void state_update_original(unsigned long * state,
  const unsigned char * key, unsigned int number_of_steps) {

  unsigned long i;
  unsigned long t0, t1, t2, t3;

  //in each iteration, we compute 128 rounds of the state update function. 
  for (i = 0; i < number_of_steps; i = i + 128) {
    t0 = (state[1] >> 15) | (state[2] << 17); // 47 = 1*32+15 
    t1 = (state[2] >> 6) | (state[3] << 26); // 47 + 23 = 70 = 2*32 + 6 
    t2 = (state[2] >> 21) | (state[3] << 11); // 47 + 23 + 15 = 85 = 2*32 + 21      
    t3 = (state[2] >> 27) | (state[3] << 5); // 47 + 23 + 15 + 6 = 91 = 2*32 + 27 
    state[0] ^= t0 ^ (~(t1 & t2)) ^ t3 ^ ((unsigned long * ) key)[0];

    t0 = (state[2] >> 15) | (state[3] << 17);
    t1 = (state[3] >> 6) | (state[0] << 26);
    t2 = (state[3] >> 21) | (state[0] << 11);
    t3 = (state[3] >> 27) | (state[0] << 5);
    state[1] ^= t0 ^ (~(t1 & t2)) ^ t3 ^ ((unsigned long * ) key)[1];

    t0 = (state[3] >> 15) | (state[0] << 17);
    t1 = (state[0] >> 6) | (state[1] << 26);
    t2 = (state[0] >> 21) | (state[1] << 11);
    t3 = (state[0] >> 27) | (state[1] << 5);
    state[2] ^= t0 ^ (~(t1 & t2)) ^ t3 ^ ((unsigned long * )(key))[2];

    t0 = (state[0] >> 15) | (state[1] << 17);
    t1 = (state[1] >> 6) | (state[2] << 26);
    t2 = (state[1] >> 21) | (state[2] << 11);
    t3 = (state[1] >> 27) | (state[2] << 5);
    state[3] ^= t0 ^ (~(t1 & t2)) ^ t3 ^ ((unsigned long * ) key)[3];
  }
}

unsigned int CMPT2(unsigned int ai, unsigned int bi) {

  return ((ai >> 6) | (bi << 10));
}

unsigned int CMPT3(unsigned int ai, unsigned int bi) {

  return ((ai >> 5) | (bi << 11));
}

unsigned int CMPT4(unsigned int ai, unsigned int bi) {

  return ((ai >> 11) | (bi << 5));
}

unsigned int CMPT1(unsigned int ai, unsigned int bi) {

  return ((ai >> 15) | (bi << 1));
}

void test_state_update() {
  uint32_t state[4], key[4];
  int i;

  // initialize state and key
  for (i = 0; i < 4; i++) {
    state[i] = 0x01234567UL;
    key[i] = 0x01234567UL;
  }

  printf("==== C implementation: ====\n");
  print_state(state);
  state_update_original((unsigned long * ) state, (unsigned char * ) key, NROUND2);
  print_state(state);

  printf("\n");

  // initialize state and key
  for (i = 0; i < 4; i++) {
    state[i] = 0x01234567UL;
    key[i] = 0x01234567UL;
  }

  printf("==== assembly implementation: ====\n");
  print_state(state);
  state_update((uint16_t * ) state, (uint16_t * ) key, NROUND2);
  print_state(state);
}

int main(void) {
  //  test_state_update();
  return 0;
}