#pragma once

#define DEFAULT_CYCLES		32

namespace Amendux {

	class XTEA
	{
		uint32_t key[4];
		size_t nsize;
		unsigned int cycles = DEFAULT_CYCLES;

		void ShiftKey(const unsigned char keyin[]);
		unsigned char *Pad(const unsigned char c[]);
		void EncryptBlock(uint32_t v[2]);
		void DecryptBlock(uint32_t v[2]);
		void EncryptRaw(unsigned char c[]);
		void DecryptRaw(unsigned char c[]);

	public:
		XTEA(const unsigned char k[]);

		unsigned char *Encrypt(const unsigned char *input);
		unsigned char *Decrypt(unsigned char *input, size_t size);

		std::wstring Encrypt(std::wstring input);
		std::string Encrypt(std::string input);

		inline void setCycles(unsigned int cycles) {
			this->cycles = cycles;
		}

		inline size_t size() {
			return nsize;
		}

		inline void clear() {
			nsize = 0;
		}

	};

}
