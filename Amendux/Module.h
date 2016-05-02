#pragma once

#include "ModuleLoader.h"

namespace Amendux {

	class Module
	{
	public:
		Module(const std::wstring& name, ModuleLoader *loader) {
			loader->RegisterModule(name, this);
		}

		virtual DWORD Run() = 0;
	};

}
