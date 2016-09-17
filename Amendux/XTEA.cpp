#include "stdafx.h"

#if 0
void encrypt(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
	uint32_t v0 = v[0];
	uint32_t v1 = v[1];
	uint32_t sum = 0, delta = 0x9E3779B9;

	for (unsigned int i = 0; i < num_rounds; i++) {
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
	}

	v[0] = v0;
	v[1] = v1;
}

void decrypt(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
	uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta*num_rounds;

	for (unsigned int i = 0; i < num_rounds; i++) {
		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
		sum -= delta;
		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
	}

	v[0] = v0;
	v[1] = v1;
}
#endif
