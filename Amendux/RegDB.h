#pragma once

#include <iostream>

namespace Amendux {

	class RegDB
	{
	public:
		static HKEY createKey(HKEY kRoot, std::wstring location);
		static DWORD getValue(HKEY hKey, LPCTSTR lpValue);

		template <typename T>
		static void setValue(HKEY hKey, DWORD type, LPCTSTR lpValue, T data, DWORD datalen = sizeof(T))
		{
			LONG nError = RegSetValueEx(hKey, lpValue, NULL, type, (LPBYTE)data, datalen);

			if (nError)
				std::cout << "Error: " << nError << " Could not set registry value: " << (char*)lpValue << std::endl;
		}

	};

}
