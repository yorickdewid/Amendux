#include "stdafx.h"
#include "Log.h"
#include "Infect.h"

using namespace Amendux;

Infect *Infect::s_Infect = nullptr;

Infect::Infect()
{
	Log::Info(L"Infect", L"Initialize infect class");
}


Infect::~Infect()
{
	Log::Info(L"Infect", L"Terminate infect class");
}


void Infect::InitClass()
{
	// 
}
