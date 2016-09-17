#pragma once

#include "Module.h"

namespace Amendux {

	class Screen : Module
	{
		//

	public:
		Screen(ModuleLoader *loader) : Module(L"screen", loader) {}

		DWORD Run();
	};

}

