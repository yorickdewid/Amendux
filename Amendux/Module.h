#pragma once

#include "ModuleLoader.h"

#define MOD_OK   42

namespace Amendux {

	class Module
	{
		std::map<std::wstring, std::wstring> parameters;

	public:
		Module(const std::wstring& name, ModuleLoader *loader) {
			loader->RegisterModule(name, this);
		}

		void PushParameters(std::map<std::wstring, std::wstring> params) {
			this->parameters = params;
		}
		
		virtual DWORD Run() = 0;
	};

}
