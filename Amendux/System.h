#pragma once

#include "Module.h"

namespace Amendux {

	class System : Module
	{
	public:
		System(ModuleLoader *loader) : Module(L"system", loader) {}

		DWORD Run();
	};

}

