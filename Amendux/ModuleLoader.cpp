#include "stdafx.h"
#include "Log.h"
#include "ModuleLoader.h"

using namespace Amendux;

ModuleLoader *ModuleLoader::s_ModuleLoader = nullptr;

ModuleLoader::ModuleLoader()
{
	Log::Info(L"Module", L"Initialize module class");
}


ModuleLoader::~ModuleLoader()
{
	Log::Info(L"Module", L"Terminate module class");
}


void ModuleLoader::InitClass()
{
	// 
}
