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
}
