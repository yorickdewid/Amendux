#include "stdafx.h"
#include "RegDB.h"

using namespace Amendux;

RegDB::RegDB()
{
}


HKEY RegDB::createKey(HKEY kRoot, std::wstring location)
{
	std::wofstream log;

	log.open("debug2.log");
	log << "[RegDB] Start\n";

	HKEY hKey;
	LONG nError = RegOpenKeyEx(kRoot, location.c_str(), NULL, KEY_ALL_ACCESS, &hKey);

	if (nError == ERROR_FILE_NOT_FOUND)
		nError = RegCreateKeyEx(kRoot, location.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

	if (nError)
		log << "Error: " << nError << " Could not find or create " << location.c_str() << std::endl;

	return hKey;
}


void RegDB::setValue(HKEY hKey, LPCTSTR lpValue, DWORD data)
{
	LONG nError = RegSetValueEx(hKey, lpValue, NULL, REG_DWORD, (LPBYTE)&data, sizeof(DWORD));

	if (nError)
		std::cout << "Error: " << nError << " Could not set registry value: " << (char*)lpValue << std::endl;
}

DWORD RegDB::getValue(HKEY hKey, LPCTSTR lpValue)
{
	DWORD data;		DWORD size = sizeof(data);	DWORD type = REG_DWORD;
	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError == ERROR_FILE_NOT_FOUND)
		data = 0; // The value will be created and set to data next time SetVal() is called.
	else if (nError)
		std::cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << std::endl;

	return data;
}


RegDB::~RegDB()
{
}
