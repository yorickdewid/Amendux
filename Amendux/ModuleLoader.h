#pragma once

#include "CoreInterface.h"

namespace Amendux {

	class ModuleLoader : CoreInterface
	{
		static ModuleLoader *s_ModuleLoader;
		std::map<std::wstring, void *> moduleList;
		void InitClass();

	public:
		ModuleLoader();
		~ModuleLoader();

		bool RunModule(const std::wstring& modName, std::map<std::wstring, std::wstring> params);

		void RegisterModule(const std::wstring& modName, void *pMod) {
			moduleList[modName] = pMod;
		}

		static ModuleLoader *Current() {
			if (!s_ModuleLoader) {
				s_ModuleLoader = new ModuleLoader;
			}

			return s_ModuleLoader;
		}

		static void Init() {
			if (!s_ModuleLoader) {
				s_ModuleLoader = new ModuleLoader;
			}

			s_ModuleLoader->InitClass();
		}

		static void Terminate() {
			if (s_ModuleLoader) {
				delete s_ModuleLoader;
				s_ModuleLoader = nullptr;
			}
		}
	};

}
