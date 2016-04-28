#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "ModuleLoader.h"

using namespace Amendux;

ModuleLoader *ModuleLoader::s_ModuleLoader = nullptr;

ModuleLoader::ModuleLoader()
{
	Log::Info(L"ModuleLoader", L"Initialize module loader class");
}


ModuleLoader::~ModuleLoader()
{
	Log::Info(L"ModuleLoader", L"Terminate module loader class");
}


void ModuleLoader::InitClass()
{
	if (!Config::Current()->CanRunModules()) {
		return;
	}

	//TODO load static modules

	//TODO load dynamic modules

	// SHELLMOD_API int fnShellMod();
	/*{
		typedef int (*DLLPROC)();
		
		HINSTANCE hInstDLL = LoadLibrary(L"ShellMod.dll");
		if (hInstDLL) {
			DLLPROC fnRunMod = (DLLPROC)GetProcAddress(hInstDLL, "fnShellMod");

			if (fnRunMod) {
				fnRunMod();
			}

			FreeLibrary(hInstDLL);
		}
	}*/
}
