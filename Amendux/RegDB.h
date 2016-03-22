#pragma once

#include <iostream>

namespace Amendux {

	class RegDB
	{
	public:
		RegDB();
		~RegDB();

		static HKEY createKey(HKEY kRoot, std::wstring location);
		static void setValue(HKEY hKey, LPCTSTR lpValue, DWORD data);
		static DWORD getValue(HKEY hKey, LPCTSTR lpValue);
	};

}
