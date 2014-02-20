#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

const uint64_t KEY = 0x0000000000000000;
//const uint64_t PLAINTEXT = 0x1234567890ABCDEF;
const uint64_t PLAINTEXT = 0xFFFFFFFFFFFFFFFF;
const int Nr = 12;
//const int Nr = 2;


// const int SBOX[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
const uint8_t SBOX[] = {0x7, 0x4, 0xA, 0x9, 0x1, 0xF, 0xB, 0x0, 0xC, 0x3, 0x2, 0x6, 0x8, 0xD, 0xE, 0x5};

void printBin64(uint64_t n) {
	int i;
	for (i = 63; i >= 0; i--) {
		printf("%d", (int) (n >> i) & 0x1);
		if (i % 4 == 0) {
			printf(" ");
		}
	}
	printf("\n");
}

void printBin8(uint8_t n) {
	int i;
	for (i = 7; i >= 0; i--) {
		printf("%d", (int) (n >> i) & 0x1);
		if (i % 4 == 0) {
			printf(" ");
		}
	}
	printf("\n");
}

uint8_t galois_x4_1(uint8_t a) {
	int i;
	for (i = 7; i >= 0; i--) {
		if (a >> i & 0x1) {
			a ^= 0x11 << (i - 4); // shift n to the same bit as the current bit in 'a'
		}
	}
	return a;
}

uint64_t addRoundKey(uint64_t state, uint64_t sk) {
	return state ^ sk;
}

uint64_t subNibbles(uint64_t state) {
	uint64_t result = 0;
	int i;
	for (i = 15; i >= 0; i--) {
		result = result << 4;
		result += SBOX[(state & ((uint64_t) 0xF << 4*i)) >> 4*i];
		//printBin(result);
	}
	return result;
}

uint64_t rotateLeft(uint64_t n) {
	return ((n >> 63) & 0x1) | (n << 1);
}

uint64_t rotateNibbles(uint64_t state) {
	int i;
	for(i = 0; i < 16;i++) {
		state = rotateLeft(state);
	}
	return state;
}

uint64_t mixNibbles(uint64_t state) {
	//This fits _exactly_ in the registers.

	uint8_t c01_1 = galois_x4_1((uint8_t) (state >> 56) & 0xFF);
	uint8_t c01_2 = c01_1 << 1;
	uint8_t c01_3 = c01_1 ^ c01_2;
	uint8_t c23_1 = galois_x4_1((uint8_t) (state >> 48) & 0xFF);
	uint8_t c23_2 = c23_1 << 1;
	uint8_t c23_3 = c23_1 ^ c23_2;
	uint8_t c45_1 = galois_x4_1((uint8_t) (state >> 40) & 0xFF);
	uint8_t c45_2 = c45_1 << 1;
	uint8_t c45_3 = c45_1 ^ c45_2;
	uint8_t c67_1 = galois_x4_1((uint8_t) (state >> 32) & 0xFF);
	uint8_t c67_2 = c67_1 << 1;
	uint8_t c67_3 = c67_1 ^ c67_2;
	uint8_t c89_1 = galois_x4_1((uint8_t) (state >> 24) & 0xFF);
	uint8_t c89_2 = c89_1 << 1;
	uint8_t c89_3 = c89_1 ^ c89_2;
	uint8_t cAB_1 = galois_x4_1((uint8_t) (state >> 16) & 0xFF);
	uint8_t cAB_2 = cAB_1 << 1;
	uint8_t cAB_3 = cAB_1 ^ cAB_2;
	uint8_t cCD_1 = galois_x4_1((uint8_t) (state >> 8) & 0xFF);
	uint8_t cCD_2 = cCD_1 << 1;
	uint8_t cCD_3 = cCD_1 ^ cCD_2;
	uint8_t cEF_1 = galois_x4_1((uint8_t) state & 0xFF);
	uint8_t cEF_2 = cEF_1 << 1;
	uint8_t cEF_3 = cEF_1 ^ cEF_2;
	
	printf("cdingen1: %"PRIX64 "\n",c23_1);
	printf("cdingen2: %"PRIX64 "\n",c23_2);
	printf("cdingen3: %"PRIX64 "\n",c23_3);

	uint8_t s01 = galois_x4_1((uint8_t) c01_2 ^ c23_3 ^ c45_1 ^ c67_1);
	uint8_t s23 = galois_x4_1((uint8_t) c01_1 ^ c23_2 ^ c45_3 ^ c67_1);
	uint8_t s45 = galois_x4_1((uint8_t) c01_1 ^ c23_1 ^ c45_2 ^ c67_3);
	uint8_t s67 = galois_x4_1((uint8_t) c01_3 ^ c23_1 ^ c45_1 ^ c67_2);
	uint8_t s89 = galois_x4_1((uint8_t) c89_2 ^ cAB_3 ^ cCD_1 ^ cEF_1);
	uint8_t sAB = galois_x4_1((uint8_t) c89_1 ^ cAB_2 ^ cCD_3 ^ cEF_1);
	uint8_t sCD = galois_x4_1((uint8_t) c89_1 ^ cAB_1 ^ cCD_2 ^ cEF_3);
	uint8_t sEF = galois_x4_1((uint8_t) c89_3 ^ cAB_1 ^ cCD_1 ^ cEF_2);

	return (((uint64_t) s01) << 56) ^ (((uint64_t) s23) << 48) ^ (((uint64_t) s45) << 40) ^ (((uint64_t) s67) << 32) ^ (((uint64_t) s89) << 24) ^ (((uint64_t) sAB) << 16) ^ (((uint64_t) sCD) << 8) ^ (uint64_t) sEF;
}

uint64_t keySchedule(uint64_t sk, uint8_t i) {
	uint8_t r0 = ((uint8_t) (sk >> 56) & 0xFF);
	uint8_t r1 = ((uint8_t) (sk >> 48) & 0xFF);
	uint8_t r2 = ((uint8_t) (sk >> 40) & 0xFF);
	uint8_t r3 = ((uint8_t) (sk >> 32) & 0xFF);
	uint8_t r4 = ((uint8_t) (sk >> 24) & 0xFF);
	uint8_t r5 = ((uint8_t) (sk >> 16) & 0xFF);
	uint8_t r6 = ((uint8_t) (sk >> 8) & 0xFF);
	uint8_t r7 = ((uint8_t) sk & 0xFF);
	
	// <implicit swap> now the order is 1230 5674
	
	r1 ^= r5;
	r2 ^= r6;
	r3 ^= r7;
	r0 ^= r4;
	
	return (((uint64_t) r5) << 56) ^ (((uint64_t) r6) << 48) ^ (((uint64_t) (r7 ^ i)) << 40) ^ (((uint64_t) r4) << 32) ^ (((uint64_t) r1) << 24)
		 ^ (((uint64_t) SBOX[(r2 >> 4) & 0xF]) << 20) ^ (((uint64_t) SBOX[r2 & 0xF]) << 16)
		 ^ (((uint64_t) SBOX[(r3 >> 4) & 0xF]) << 12) ^ (((uint64_t) SBOX[r3 & 0xF]) << 8)
		 ^ (uint64_t) r0;
}
	
	

uint64_t KLEIN(uint64_t key, uint64_t plaintext) {
	uint64_t sk = key;
	uint64_t state = plaintext;
	uint8_t i;
	for (i = 1; i <= Nr; i++) {
		state = addRoundKey(state, sk);
		printf("na addround   :  %" PRIX64 "\n", state);
		state = subNibbles(state);
		printf("na subnibbles :  %" PRIX64 "\n", state);
		state = rotateNibbles(state);
		printf("na rotbnibbles:  %" PRIX64 "\n", state);
		state = mixNibbles(state);
		printf("na mixbnibbles:  %" PRIX64 "\n", state);
		sk = keySchedule(sk, i);
		printf("roundkey      :  %" PRIX64 "\n", sk);
	}
	state = addRoundKey(state, sk);
	return state;
}

int main() {
	//printf("Key: %" PRIX64 "\n", KEY);
	printf("Ciphertext: %" PRIX64 "\n", KLEIN(KEY, PLAINTEXT));
	printf("Plaintext:  %" PRIX64 "\n", PLAINTEXT);
	printBin8(galois_x4_1(254));
	return 0;
}
