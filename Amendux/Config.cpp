#include "stdafx.h"
#include "Log.h"
#include "RegDB.h"
#include "Config.h"

using namespace Amendux;

Config *Config::s_Config = nullptr;

Config::Config()
{
	Log::Info(L"Config", L"Initialize config class");

	// Encrypt module settings
	{
		// HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux\\Crypt");
		// LPBYTE dRsPubkey = RegDB::getValue<LPBYTE>(hRoot, REG_BINARY, L"clientPubkey", crypto_box_PUBLICKEYBYTES);
		// LPBYTE dRsPrivkey = RegDB::getValue<LPBYTE>(hRoot, REG_BINARY, L"clientPrivkey", crypto_box_SECRETKEYBYTES);
		// if (!dRsPubkey || !dRsPrivkey) {
		// encrypt.genLocalKeypair();

		// RegDB::setValue<LPBYTE>(hRoot, REG_BINARY, L"clientPubkey", encrypt.clientPublickey(), crypto_box_PUBLICKEYBYTES);
		// RegDB::setValue<LPBYTE>(hRoot, REG_BINARY, L"clientPrivkey", encrypt.clientPrivatekey(), crypto_box_SECRETKEYBYTES);

		// Log::Info(L"Config", L"Client keys generated and set");
		// }
		// else {
		// encrypt.setLocalKeypair(dRsPubkey, dRsPrivkey);
		// }
	}
}


Config::~Config()
{
	Log::Info(L"Config", L"Terminate config class");
}


void Config::InitClass()
{
	Log::Info(L"Config", L"Mode: " + std::to_wstring((int)currentMode));

	mainThreadId = ::GetCurrentThreadId();

	switch (currentMode) {
		case Amendux::OperationMode::BASE:

			SetupPersistentConfig();
			SetupDataDir();

			LogEnvironment();
			break;
		case Amendux::OperationMode::UPDATE:

			CheckObsoleteConfig();
			ApplyUpdate();

			LogEnvironment();
			break;
		case Amendux::OperationMode::ELIMINATE:
			break;
		case Amendux::OperationMode::GUARD:
			
			BasicConfig();

			LogEnvironment();
			break;
		default:
			break;
	}
}


void Config::LogEnvironment()
{
#ifdef DEBUG
	PWCHAR sUserDir;

	Log::Info(L"Config", L"[Env] PID: " + std::to_wstring(Util::currentProcessId()));
	Log::Info(L"Config", L"[Env] Version: " + Config::getVersion());
	Log::Info(L"Config", L"[Env] GUID: " + Config::instanceGUID);
	Log::Info(L"Config", L"[Env] Windows version: " + Util::winver());
	Log::Info(L"Config", L"[Env] CPU cores: " + std::to_wstring(Util::cpuCores()));
	Log::Info(L"Config", L"[Env] Max memory: " + std::to_wstring(Util::maxmem()));
	Log::Info(L"Config", L"[Env] User: " + Util::user());
	Log::Info(L"Config", L"[Env] Computer: " + Util::machine());
	Log::Info(L"Config", L"[Env] Screen resolution: " + Util::windowResolution());
	Log::Info(L"Config", L"[Env] Timezone: " + Util::timezoneName());

	sUserDir = Util::getDirectory(Util::Directory::USER_STARTUP);
	Log::Info(L"Config", L"[Env] User startup: " + std::wstring(sUserDir));
#endif
}


void Config::SetupDataDir()
{
	if (!Util::createDirectory(DataDirectory())) {
		Log::Error(L"Config", L"Cannot create data directory");

		bSuccess = false;
	}
}


void Config::SetupPersistentConfig()
{
	HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux");
	LPBYTE dRsTempPath = RegDB::getValue<LPBYTE>(hRoot, REG_SZ, L"Instance", 39 * sizeof(wchar_t));
	if (!dRsTempPath) {
		std::wstring guid = Util::generateUUID();
		instanceGUID = guid;

		LPCWSTR newTempPath = guid.c_str();
		RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, L"Instance", newTempPath, static_cast<DWORD>(Util::bytesInWCharStr(newTempPath)));
	} else {
		instanceGUID = std::wstring(reinterpret_cast<PWCHAR>(dRsTempPath), 76 / sizeof(wchar_t));
	}

	DWORD procInit = 1;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"InitProcedure", &procInit, sizeof(DWORD));

	DWORD *dRsRunCount = (DWORD *)RegDB::getValue<LPBYTE>(hRoot, REG_DWORD, L"RunCount", sizeof(DWORD));
	if (!dRsRunCount) {
		DWORD RunCount = 1;
		RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"RunCount", &RunCount, sizeof(DWORD));
	} else {
		DWORD RunCount = *dRsRunCount + 1;
		RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"RunCount", &RunCount, sizeof(DWORD));
	}

	DWORD execMode = 3;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"ExecMode", &execMode, sizeof(DWORD));

	DWORD build = clientVersion;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"Build", &build, sizeof(DWORD));

	RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, L"DisplayName", L"Amendux", 7 * sizeof(wchar_t));

	RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, L"Path", DataDirectory().c_str(), (DWORD)(DataDirectory().size() * sizeof(wchar_t)));

	BYTE flags = 0xfe;
	RegDB::setValue<LPBYTE>(hRoot, REG_BINARY, L"Flags", &flags, sizeof(BYTE));
}


void Config::CheckObsoleteConfig()
{
	HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux");
	LPBYTE dRsTempPath = RegDB::getValue<LPBYTE>(hRoot, REG_SZ, L"Instance", 39 * sizeof(wchar_t));
	if (!dRsTempPath) {
		bSuccess = false;
		return;
	}
	
	instanceGUID = std::wstring(reinterpret_cast<PWCHAR>(dRsTempPath), 76 / sizeof(wchar_t));

	DWORD *procInit = (DWORD *) RegDB::getValue<LPBYTE>(hRoot, REG_DWORD, L"InitProcedure", sizeof(DWORD));
	if (!*procInit || *procInit != 1) {
		bSuccess = false;
		return;
	}

	DWORD *build = (DWORD *) RegDB::getValue<LPBYTE>(hRoot, REG_DWORD, L"Build", sizeof(DWORD));
	if (!*build || *build >= clientVersion) {
		bSuccess = false;
		return;
	}
}


void Config::ApplyUpdate()
{
	if (!bSuccess) {
		return;
	}
	
	Log::Info(L"Config", L"Applying update");

	HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux");
	std::wstring startupDir = Util::getDirectory(Util::Directory::USER_STARTUP);
	std::wstring curExec = Util::currentModule();
	std::wstring appDir = Config::Current()->DataDirectory();
	std::wstring curDir = Util::CurrentDirectory();

	if (appDir == curDir) {
		return;
	}

	appDir += L"\\Amendux.exe";
	startupDir += L"\\AmenduxGuard.exe";

	Util::deleteFile(appDir.c_str());
	Util::CopyFile((wchar_t *)curExec.c_str(), (wchar_t *)appDir.c_str());

	Util::deleteFile(startupDir.c_str());
	Util::CopyFile((wchar_t *)curExec.c_str(), (wchar_t *)startupDir.c_str());

	DWORD build = clientVersion;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"Build", &build, sizeof(DWORD));

	PostQuitMessage(0);
}


void Config::BasicConfig()
{
	HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Amendux");
	LPBYTE dRsTempPath = RegDB::getValue<LPBYTE>(hRoot, REG_SZ, L"Instance", 39 * sizeof(wchar_t));
	if (!dRsTempPath) {
		bSuccess = false;
		return;
	}

	instanceGUID = std::wstring(reinterpret_cast<PWCHAR>(dRsTempPath), 76 / sizeof(wchar_t));

	DWORD *procInit = (DWORD *)RegDB::getValue<LPBYTE>(hRoot, REG_DWORD, L"InitProcedure", sizeof(DWORD));
	if (!*procInit || *procInit != 1) {
		bSuccess = false;
		return;
	}
}
