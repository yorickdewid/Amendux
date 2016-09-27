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
		
		std::wstring GetParameter(const std::wstring key) const {
			if (parameters.find(key) == parameters.end()) {
				return L"";
			}

			return parameters.find(key)->second;
		}

		virtual DWORD Run() = 0;
	};

}
