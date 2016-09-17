#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "Shell.h"
#include "Screen.h"
#include "Thread.h"
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


void ModuleLoader::RunModule(const std::wstring& modName) {
	Module *mod = (Module *)moduleList[modName];
	if (mod) {
		Thread<Module> *thread = new Thread<Module>(mod, &Module::Run);
		if (!thread->Start()) {
			Log::Error(L"Candcel", L"Cannot spawn module " + modName);
		}
	}
}


void ModuleLoader::InitClass()
{
	if (!Config::Current()->CanRunModules()) {
		return;
	}

	// Register modules
#ifdef MODSHELL_ENABLED
	RegisterModule(L"shell", new Shell(this));
#endif

#ifdef MODSCREEN_ENABLED
	RegisterModule(L"screen", new Screen(this));
#endif
}
