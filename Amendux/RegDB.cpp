#include "stdafx.h"
#include "RegDB.h"

using namespace Amendux;

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
