#include "stdafx.h"
#include "RegDB.h"

#include <iostream>

using namespace Amendux;

HKEY RegDB::createKey(HKEY kRoot, std::wstring location)
{
	HKEY hKey;
	LONG nError = RegOpenKeyEx(kRoot, location.c_str(), NULL, KEY_ALL_ACCESS, &hKey);

	if (nError == ERROR_FILE_NOT_FOUND)
		nError = RegCreateKeyEx(kRoot, location.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

	if (nError)
		std::cerr << "Error: " << nError << " Could not find or create " << location.c_str() << std::endl;

	return hKey;
}


VOID RegDB::deleteKey(HKEY kRoot, std::wstring location)
{
	LONG nError = RegDeleteKey(kRoot, location.c_str());
}