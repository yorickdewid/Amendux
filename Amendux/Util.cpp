#include "stdafx.h"
#include "Util.h"

#include <Shlobj.h>
#include <Lmcons.h>
#include <VersionHelpers.h>
#include <iostream>

using namespace Amendux;

PWCHAR Util::getDirectory(Util::Directory folderId)
{
	KNOWNFOLDERID kfid;
	WCHAR *localDocPath = new WCHAR[128];
	
	switch (folderId) {
		case Util::Directory::USER_DOCUMENTS:
			kfid = FOLDERID_Documents;
			break;
		case Util::Directory::USER_CONTACT:
			kfid = FOLDERID_Contacts;
			break;
		case Util::Directory::USER_DESKTOP:
			kfid = FOLDERID_Desktop;
			break;
		case Util::Directory::USER_DOWNLOAD:
			kfid = FOLDERID_Downloads;
			break;
		case Util::Directory::USER_FAVORITE:
			kfid = FOLDERID_Favorites;
			break;
		case Util::Directory::USER_APPDATA:
			kfid = FOLDERID_LocalAppData;
			break;
		case Util::Directory::USER_PICTURES:
			kfid = FOLDERID_Pictures;
			break;
		case Util::Directory::USER_MUSIC:
			kfid = FOLDERID_Music;
			break;
		case Util::Directory::USER_VIDEOS:
			kfid = FOLDERID_Videos;
			break;
		default:
			break;
	}

	if (SHGetKnownFolderPath(kfid, 0, NULL, &localDocPath) != S_OK) {
		std::cerr << "Cannot get folder" << std::endl;
	}

	return localDocPath;
}


std::wstring Util::generateUUID()
{
	GUID gidReference;
	wchar_t szGuidW[40];
	HRESULT hCreateGuid = CoCreateGuid(&gidReference);
	StringFromGUID2(gidReference, szGuidW, 40);
	return std::wstring(szGuidW);
}


std::wstring Util::user()
{
	TCHAR username[UNLEN + 1];
	DWORD size = UNLEN + 1;
	GetUserName(username, &size);
	return std::wstring(username);
}


std::wstring Util::machine()
{
	TCHAR name[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(name, &size);
	return std::wstring(name);
}


std::wstring Util::winver()
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


DWORD Util::cpuCores()
{
	SYSTEM_INFO siSysInfo;

	GetSystemInfo(&siSysInfo);

	return siSysInfo.dwNumberOfProcessors;
}


DWORD Util::maxmem()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	return static_cast<DWORD>(statex.ullTotalPhys / (1024 * 1024));
}
