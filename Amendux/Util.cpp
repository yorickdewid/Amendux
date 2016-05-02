#include "stdafx.h"
#include "Util.h"

#include <Shlobj.h>
#include <Lmcons.h>
#include <VersionHelpers.h>
#include <iostream>

PWCHAR Amendux::Util::getDirectory(Directory folderId)
{
	KNOWNFOLDERID kfid;
	WCHAR *localDocPath = new WCHAR[128];
	
	switch (folderId) {
		case Directory::USER_DOCUMENTS:
			kfid = FOLDERID_Documents;
			break;
		case Directory::USER_CONTACT:
			kfid = FOLDERID_Contacts;
			break;
		case Directory::USER_DESKTOP:
			kfid = FOLDERID_Desktop;
			break;
		case Directory::USER_DOWNLOAD:
			kfid = FOLDERID_Downloads;
			break;
		case Directory::USER_FAVORITE:
			kfid = FOLDERID_Favorites;
			break;
		case Directory::USER_APPDATA:
			kfid = FOLDERID_LocalAppData;
			break;
		case Directory::USER_PICTURES:
			kfid = FOLDERID_Pictures;
			break;
		case Directory::USER_MUSIC:
			kfid = FOLDERID_Music;
			break;
		case Directory::USER_VIDEOS:
			kfid = FOLDERID_Videos;
			break;
		case Directory::USER_STARTUP:
			kfid = FOLDERID_Startup;
			break;
		default:
			break;
	}

	if (SHGetKnownFolderPath(kfid, 0, NULL, &localDocPath) != S_OK) {
		std::cerr << "Cannot get folder" << std::endl;
	}

	return localDocPath;
}


std::wstring Amendux::Util::generateUUID()
{
	GUID gidReference;
	wchar_t szGuidW[40];
	HRESULT hCreateGuid = CoCreateGuid(&gidReference);
	StringFromGUID2(gidReference, szGuidW, 40);
	return std::wstring(szGuidW);
}


std::wstring Amendux::Util::user()
{
	TCHAR username[UNLEN + 1];
	DWORD size = UNLEN + 1;
	GetUserName(username, &size);
	return std::wstring(username);
}


std::wstring Amendux::Util::machine()
{
	TCHAR name[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(name, &size);
	return std::wstring(name);
}


std::wstring Amendux::Util::winver()
{
	if (IsWindowsServer()) {
		return L"Windows Server";
	}

	// At some point this becomes available
	/*if (IsWindows10OrGreater()) {
		return L"Windows 10";
	}*/

	if (IsWindows8Point1OrGreater()) {
		return L"Windows 8.1";
	}

	if (IsWindows8OrGreater()) {
		return L"Windows 8";
	}

	if (IsWindows7SP1OrGreater()) {
		return L"Windows 7 SP1";
	}

	if (IsWindows7OrGreater()) {
		return L"Windows 7";
	}

	if (IsWindowsVistaSP2OrGreater()) {
		return L"Windows Vista SP2";
	}

	if (IsWindowsVistaSP1OrGreater()) {
		return L"Windows Vista SP1";
	}

	if (IsWindowsVistaOrGreater()) {
		return L"Windows Vista";
	}

	if (IsWindowsXPSP3OrGreater()) {
		return L"Windows XP SP3";
	}

	if (IsWindowsXPSP2OrGreater()) {
		return L"Windows XP SP2";
	}

	if (IsWindowsXPSP1OrGreater()) {
		return L"Windows XP SP1";
	}

	if (IsWindowsXPOrGreater()) {
		return L"Windows XP";
	}

	return L"Other";
}


DWORD Amendux::Util::cpuCores()
{
	SYSTEM_INFO siSysInfo;

	GetSystemInfo(&siSysInfo);

	return siSysInfo.dwNumberOfProcessors;
}


DWORD Amendux::Util::maxmem()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	// Return memory size in MB
	return static_cast<DWORD>(statex.ullTotalPhys / (1024 * 1024));
}


std::wstring Amendux::Util::tempFile()
{
	DWORD dwRetVal = 0;
	UINT uRetVal = 0;

	WCHAR szTempFileName[MAX_PATH];
	WCHAR lpTempPathBuffer[MAX_PATH];
	WCHAR lpPrex[4];

	// Gets the temp path env string (no guarantee it's a valid path).
	dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
	if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
		return nullptr;
	}

	Amendux::Util::generateString(lpPrex, 3);

	// Generates a temporary file name.
	uRetVal = GetTempFileName(lpTempPathBuffer, lpPrex, 0, szTempFileName);
	if (uRetVal == 0) {
		return nullptr;
	}

	return szTempFileName;
}


HRESULT Amendux::Util::CreateHardLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink, LPCWSTR lpszDesc)
{
	HRESULT hres;
	IShellLink* psl;

	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if (SUCCEEDED(hres)) {
		IPersistFile* ppf;

		// Set the path to the shortcut target and add the description. 
		psl->SetPath(lpszPathObj);
		psl->SetDescription(lpszDesc);

		// Query IShellLink for the IPersistFile interface, used for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres)) {
			// WCHAR wsz[MAX_PATH];

			// Ensure that the string is Unicode. 
			// MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);

			// Add code here to check return value from MultiByteWideChar 
			// for success.

			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(lpszPathLink, TRUE);
			ppf->Release();
		}
		psl->Release();
	}

	return hres;
}


bool Amendux::Util::CreateSoftLink(LPTSTR lpSymlinkFileName, LPTSTR lpTargetFileName)  //TODO inline
{
	return ::CreateSymbolicLink(lpSymlinkFileName, lpTargetFileName, 0) != NULL;
}


bool Amendux::Util::CopyFile(LPTSTR lpExistingFileName, LPTSTR lpNewFileName) //TODO inline
{
	return ::CopyFile(lpExistingFileName, lpNewFileName, TRUE) != NULL;
}


std::wstring Amendux::Util::CurrentDirectory() //TODO inline
{
	TCHAR buffer[MAX_PATH];

	::GetCurrentDirectory(MAX_PATH, buffer);

	return buffer;
}

std::map<std::wstring, std::wstring> *Amendux::Util::EnvVariables()
{
	LPTSTR lpszVariable;
	LPTCH lpvEnv;
	std::map<std::wstring, std::wstring> *envList = new std::map<std::wstring, std::wstring>;

	// Get a pointer to the environment block. 
	lpvEnv = ::GetEnvironmentStrings();

	// If the returned pointer is NULL, exit.
	if (!lpvEnv) {
		return nullptr;
	}

	// Variable strings are separated by NULL byte, and the block is 
	// terminated by a NULL byte. 
	lpszVariable = (LPTSTR)lpvEnv;

	while (*lpszVariable) {
		std::wstring envItem = std::wstring(lpszVariable);
		//_tprintf(L">>%s\n", lpszVariable);

		size_t nCnt = std::count(envItem.begin(), envItem.end(), '=');
		if (nCnt == 1) {
			size_t pos = envItem.find_first_of('=');

			(*envList)[(envItem.substr(0, pos))] = Util::trim<std::wstring>(envItem.substr(pos + 1));
		}

		// Log::Info(L"Config", L"[Env] VAR: " + x);

		lpszVariable += lstrlen(lpszVariable) + 1;
	}

	::FreeEnvironmentStrings(lpvEnv);

	return envList;
}