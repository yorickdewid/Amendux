#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "Candcel.h"
#include "System.h"

using namespace Amendux;

DWORD System::Run() {
	std::wstring op = GetParameter(L"operation");

	if (!op.compare(L"0")) {
		std::wstring msg = GetParameter(L"msg");
		std::wstring cap = GetParameter(L"cap");

		if (!msg.empty() && !cap.empty()) {
			MessageBox(NULL, msg.c_str(), cap.c_str(), MB_ICONINFORMATION);
		}
	}

	return MOD_OK;
}
