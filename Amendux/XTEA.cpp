#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "XTEA.h"

#include <assert.h>

using namespace Amendux;

XTEA::XTEA(const unsigned char k[])
{
	assert(strlen((const char *)k) >= 16);

	ShiftKey(k);
}


unsigned char *XTEA::Pad(const unsigned char c[])
{
	size_t csz = nsize = strlen((const char *)c);
	size_t rs = csz % 8;
	if (rs != 0) {
		nsize = (csz + (8 - (csz % 8)));
	}

	unsigned char *cv = (unsigned char *)malloc(nsize);
	memset(cv, ' ', nsize);
	memcpy_s(cv, csz, c, csz);

	return cv;
}


void XTEA::EncryptBlock(uint32_t v[2]) {
	uint32_t v0 = v[0];
	uint32_t v1 = v[1];
	uint32_t sum = 0, delta = 0x9E3779B9;

	for (unsigned int i = 0; i < this->cycles; ++i) {
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
	}

	v[0] = v0;
	v[1] = v1;
}


void XTEA::DecryptBlock(uint32_t v[2]) {
	uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta * cycles;

	for (unsigned int i = 0; i < this->cycles; ++i) {
		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
		sum -= delta;
		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
	}

	v[0] = v0;
	v[1] = v1;
}


unsigned char *XTEA::Encrypt(const unsigned char *input) {
	unsigned char *cv = Pad(input);

	EncryptRaw(cv);

	return cv;
}


std::wstring XTEA::Encrypt(std::wstring input) {
	std::string t(input.begin(), input.end());
	unsigned char *cv = Pad((const unsigned char *)t.c_str());

	EncryptRaw(cv);

	std::string u((char *)cv, nsize);

	return std::wstring(u.begin(), u.end());
}


std::string XTEA::Encrypt(std::string input) {
	unsigned char *cv = Pad((const unsigned char *)input.c_str());

	EncryptRaw(cv);

	return std::string((char *)cv, nsize);
}


unsigned char *XTEA::Decrypt(unsigned char *input, size_t size) {
	nsize = size;

	DecryptRaw(input);

	return input;
}

/*
 * Convert byte array into BE bit array
 */
void XTEA::ShiftKey(const unsigned char keyin[]) {
	key[0] = (keyin[0] << 24) | (keyin[1] << 16) | (keyin[2] << 8) | keyin[3];
	key[1] = (keyin[4] << 24) | (keyin[5] << 16) | (keyin[6] << 8) | keyin[7];
	key[2] = (keyin[8] << 24) | (keyin[9] << 16) | (keyin[10] << 8) | keyin[11];
	key[3] = (keyin[12] << 24) | (keyin[13] << 16) | (keyin[14] << 8) | keyin[15];
}


void XTEA::EncryptRaw(unsigned char c[]) {
	uint32_t v[2];

	for (int i = 0; i < nsize; i += 8) {
		v[0] = (c[i] << 24) | (c[i + 1] << 16) | (c[i + 2] << 8) | c[i + 3];
		v[1] = (c[i + 4] << 24) | (c[i + 5] << 16) | (c[i + 6] << 8) | c[i + 7];

		EncryptBlock(v);

		c[i] = (v[0] >> 24) & 0xff;
		c[i + 1] = (v[0] >> 16) & 0xff;
		c[i + 2] = (v[0] >> 8) & 0xff;
		c[i + 3] = v[0] & 0xff;

		c[i + 4] = (v[1] >> 24) & 0xff;
		c[i + 5] = (v[1] >> 16) & 0xff;
		c[i + 6] = (v[1] >> 8) & 0xff;
		c[i + 7] = v[1] & 0xff;
	}
}


void XTEA::DecryptRaw(unsigned char c[]) {
	uint32_t v[2];

	for (int i = 0; i < nsize; i += 8) {
		v[0] = (c[i] << 24) | (c[i + 1] << 16) | (c[i + 2] << 8) | c[i + 3];
		v[1] = (c[i + 4] << 24) | (c[i + 5] << 16) | (c[i + 6] << 8) | c[i + 7];

		DecryptBlock(v);

		c[i] = (v[0] >> 24) & 0xff;
		c[i + 1] = (v[0] >> 16) & 0xff;
		c[i + 2] = (v[0] >> 8) & 0xff;
		c[i + 3] = v[0] & 0xff;

		c[i + 4] = (v[1] >> 24) & 0xff;
		c[i + 5] = (v[1] >> 16) & 0xff;
		c[i + 6] = (v[1] >> 8) & 0xff;
		c[i + 7] = v[1] & 0xff;
	}
}


#if 0
void xtea_test() {
	const unsigned char k[] = XTEA_KEY;
	unsigned char c[] = "WoeiKassEnzoTrol";

	XTEA *x = new XTEA(k);
	unsigned char *p = x->Encrypt(c);
	unsigned char *d = x->Decrypt(p);

	free(p);
}
#endif
