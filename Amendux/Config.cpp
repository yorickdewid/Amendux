#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"
#include "RegDB.h"
#include "Config.h"

using namespace Amendux;

void Config::Init(Encrypt& encrypt)
{
	Log::Instance()->write(L"Config", L"Initialize config module");

	HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux\\Crypt");
	LPBYTE dRsPubkey = RegDB::getValue<LPBYTE>(hRoot, REG_BINARY, L"clientPubkey", crypto_box_PUBLICKEYBYTES);
	LPBYTE dRsPrivkey = RegDB::getValue<LPBYTE>(hRoot, REG_BINARY, L"clientPrivkey", crypto_box_SECRETKEYBYTES);
	if (!dRsPubkey || !dRsPrivkey) {
		encrypt.genLocalKeypair();

		RegDB::setValue<LPBYTE>(hRoot, REG_BINARY, L"clientPubkey", encrypt.clientPublickey(), crypto_box_PUBLICKEYBYTES);
		RegDB::setValue<LPBYTE>(hRoot, REG_BINARY, L"clientPrivkey", encrypt.clientPrivatekey(), crypto_box_SECRETKEYBYTES);

		Log::Instance()->write(L"Config", L"Client keys generated and set");
	}
	else {
		encrypt.setLocalKeypair(dRsPubkey, dRsPrivkey);
	}

	hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux\\Collect");
	hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux\\Policies");

	DWORD procInit = 1;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"InitProcedure", &procInit, sizeof(DWORD));

	DWORD execMode = 3;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"ExecMode", &execMode, sizeof(DWORD));
}

void Config::Terminate()
{
	Log::Instance()->write(L"Config", L"Terminate config module");
}
