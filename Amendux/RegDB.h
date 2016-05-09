#pragma once

#include <iostream>

namespace Amendux {

	class RegDB
	{
	public:
		static HKEY createKey(HKEY kRoot, std::wstring location);
		static VOID RegDB::deleteKey(HKEY kRoot, std::wstring location);

		static HKEY createKey(HKEY kRoot, std::string location) {
			return createKey(kRoot, std::wstring(location.begin(), location.end()));
		}

		static VOID RegDB::deleteKey(HKEY kRoot, std::string location) {
			RegDB::deleteKey(kRoot, std::wstring(location.begin(), location.end()));
		}

		template <typename T>
		static void setValue(HKEY hKey, DWORD type, LPCTSTR lpValue, T data, DWORD datalen = sizeof(T)) {
			LONG nError = RegSetValueEx(hKey, lpValue, NULL, type, (LPBYTE)data, datalen);

			if (nError) { //TODO to log
				std::cout << "Error: " << nError << " Could not set registry value: " << (char*)lpValue << std::endl;
			}
		}
		
		template <typename T>
		static T getValue(HKEY hKey, DWORD type, LPCTSTR lpValue, DWORD datalen = sizeof(T)) {
			T data = new BYTE[datalen];
			LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)data, &datalen);

			if (nError == ERROR_FILE_NOT_FOUND) {
				return 0;
			} else if (nError) { //TODO to log
				std::cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << std::endl;
			}
			
			return data;
		}

		static void deleteValue(HKEY hKey, LPCTSTR lpValue) {
			LONG nError = RegDeleteValue(hKey, lpValue);

			if (nError != ERROR_SUCCESS) { //TODO to log
				std::cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << std::endl;
			}
		}

		static VOID closeKey(HKEY kRoot) {
			RegCloseKey(kRoot);
		}

	};

}
