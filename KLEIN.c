#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

const uint64_t KEY = 0x0000000000000000;
const uint64_t PLAINTEXT = 0x1234567890ABCDEF;
const int Nr = 12;

// const int SBOX[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
const int SBOX[] = {0x7, 0x4, 0xA, 0x9, 0x1, 0xF, 0xB, 0x0, 0xC, 0x3, 0x2, 0x6, 0x8, 0xD, 0xE, 0x5};

void printBin(uint64_t n) {
	int i;
	for (i = 63; i >= 0; i--) {
		printf("%d", (int) (n >> i) & 0x1);
		if (i % 4 == 0) {
			printf(" ");
		}
	}
	printf("\n");
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
		printBin(result);
	}
	return result;
}

uint64_t KLEIN(uint64_t key, uint64_t plaintext) {
	uint64_t sk = key;
	uint64_t state = plaintext;
	int i;
	for (i = 0; i < Nr; i++) {
		state = addRoundKey(state, sk);
		state = subNibbles(state);
		break;
	}
	return 0;
}

int main() {
	printf("Key: %" PRIX64 "\n", KEY);
	printf("Plaintext: %" PRIX64 "\n", PLAINTEXT);
	printf("Ciphertext: %" PRIX64 "\n", KLEIN(KEY, PLAINTEXT));
	return 0;
}