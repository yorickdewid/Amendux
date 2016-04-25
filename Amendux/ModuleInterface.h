#pragma once

#include <iostream>

namespace Amendux {

	class ModuleInterface
	{
		std::wstring modName;

	public:
		virtual void Run() = 0;
	};

}