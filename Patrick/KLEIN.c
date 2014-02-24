#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

// Test case 1
//  expected ciphertext: CDC0 B51F 1472 2BBE
const uint64_t KEY = 0x0000000000000000;
const uint64_t PLAINTEXT = 0xFFFFFFFFFFFFFFFF;
// Test case 2
//  expected ciphertext: 6456 764E 8602 E154
//const uint64_t KEY = 0xFFFFFFFFFFFFFFFF;
//const uint64_t PLAINTEXT = 0x0000000000000000;
// Test case 3
//  expected ciphertext: 5923 56C4 9971 76C8
//const uint64_t KEY = 0x1234567890ABCDEF;
//const uint64_t PLAINTEXT = 0xFFFFFFFFFFFFFFFF;
// Test case 4
//  expected ciphertext: 629F 9D6D FF95 800E
//const uint64_t KEY = 0x0000000000000000;
//const uint64_t PLAINTEXT = 0x1234567890ABCDEF;




const int Nr = 12;
// const int SBOX[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
const uint8_t SBOX[] = {0x7, 0x4, 0xA, 0x9, 0x1, 0xF, 0xB, 0x0, 0xC, 0x3, 0x2, 0x6, 0x8, 0xE, 0xD, 0x5};


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





//This implementation comes directly from http://en.wikipedia.org/wiki/Rijndael_mix_columns#Implementation_example
void gmix_column(uint64_t *r)
{
    unsigned char a[4];
    unsigned char b[4];
    unsigned char c;
    unsigned char h;

    for(c=0; c<4; c++)
    {
        a[c] = r[c];
        h = (unsigned char)((signed char)r[c] >> 7); /* arithmetic right shift, thus shifting in either zeros or ones */
        b[c] = r[c] << 1; /* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line */
        b[c] ^= 0x1B & h; /* Rijndael's Galois field */
    }

    r[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
    r[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
    r[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
    r[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */
}

uint64_t mixNibbles(uint64_t state) {
    uint64_t temp_state[8];

    temp_state[0] = (state >> 56) & 0xFF;
    temp_state[1] = (state >> 48) & 0xFF;
    temp_state[2] = (state >> 40) & 0xFF;
    temp_state[3] = (state >> 32) & 0xFF;
    temp_state[4] = (state >> 24) & 0xFF;
    temp_state[5] = (state >> 16) & 0xFF;
    temp_state[6] = (state >> 8) & 0xFF;
    temp_state[7] = (state >> 0) & 0xFF;

    gmix_column(temp_state);
    gmix_column((temp_state+4));
    

    return (
            (temp_state[0] << 56) |
            (temp_state[1] << 48) |
            (temp_state[2] << 40) |
            (temp_state[3] << 32) |
            (temp_state[4] << 24) |
            (temp_state[5] << 16) |
            (temp_state[6] << 8) |
            (temp_state[7] << 0));












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
	
	return (((uint64_t) r5) << 56) ^ (((uint64_t) r6) << 48) ^ (((uint64_t) (r7 ^ i)) << 40) ^ (((uint64_t) r4) << 32)
		 ^ (((uint64_t) r1) << 24)
		 ^ (((uint64_t) SBOX[(r2 >> 4) & 0xF]) << 20) ^ (((uint64_t) SBOX[r2 & 0xF]) << 16)
		 ^ (((uint64_t) SBOX[(r3 >> 4) & 0xF]) << 12) ^ (((uint64_t) SBOX[r3 & 0xF]) << 8)
		 ^ (uint64_t) r0;
}
	
	

uint64_t KLEIN(uint64_t key, uint64_t plaintext) {
	uint64_t sk = key;
	uint64_t state = plaintext;
	uint8_t i;
	printf("State:  %" PRIX64 "\n", state);
	for (i = 1; i <= Nr ; i++) {
        printf("Round %i\n", i);
		state = addRoundKey(state, sk);
        printf("post addRoundKey:  %" PRIX64 "\n", state);
		state = subNibbles(state);
        printf("post subNibbles:  %" PRIX64 "\n", state);
		state = rotateNibbles(state);
        printf("post rotateNibbles:  %" PRIX64 "\n", state);
		state = mixNibbles(state);
        printf("post mixNibbles:  %" PRIX64 "\n", state);
		sk = keySchedule(sk, i);
        printf("NEW KEY:  %" PRIX64 "\n", sk);
	}
	state = addRoundKey(state, sk);
	return state;
}

int main() {
	printf("Key: %" PRIX64 "\n", KEY);
	printf("Plaintext:  %" PRIX64 "\n", PLAINTEXT);
	printf("Ciphertext: %" PRIX64 "\n", KLEIN(KEY, PLAINTEXT));
	return 0;
}
