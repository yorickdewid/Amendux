#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "Process.h"
#include "Shell.h"

using namespace Amendux;

DWORD Shell::Run()
{
	std::wstring result = Process::Execute((wchar_t *)param.c_str());
	if (!result.empty()) {
		// Do something
	}

	return MOD_OK;
}
