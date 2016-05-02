#pragma once

#include "Module.h"

namespace Amendux {

	class Shell : Module
	{
		std::wstring param = L"cmd.exe /C ifconfig";

	public:
		Shell(ModuleLoader *loader) : Module(L"shell", loader) {}
	
		DWORD Run();
	};

}
