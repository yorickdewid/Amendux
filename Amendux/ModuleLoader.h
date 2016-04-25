#pragma once

#include "CoreInterface.h"

namespace Amendux {

	class ModuleLoader : CoreInterface
	{
		static ModuleLoader *s_ModuleLoader;
		void InitClass();

	public:
		ModuleLoader();
		~ModuleLoader();

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
