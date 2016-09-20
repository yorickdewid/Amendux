#include "stdafx.h"
#if 0
#include "Vigenere.h"

using namespace Amendux;

std::string Vigenere::Encrypt(std::string text) {
	std::string out;

	for (int i = 0, j = 0; i < text.length(); ++i) {
		char c = text[i];

		if (c >= 'a' && c <= 'z')
			c += 'A' - 'a';
		else if (c < 'A' || c > 'Z')
			continue;

		out += (c + key[j] - 2 * 'A') % 26 + 'A';
		j = (j + 1) % key.length();
	}

	return out;
}


std::string Vigenere::Decrypt(std::string text) {
	std::string out;

	for (int i = 0, j = 0; i < text.length(); ++i) {
		char c = text[i];

		if (c >= 'a' && c <= 'z')
			c += 'A' - 'a';
		else if (c < 'A' || c > 'Z')
			continue;

		out += (c - key[j] + 26) % 26 + 'A';
		j = (j + 1) % key.length();
	}

	return out;
}
#endif
