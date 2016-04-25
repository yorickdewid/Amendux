#pragma once

#include <iostream>

namespace Amendux {

	class Module
	{
		std::wstring modName;

	public:
		virtual void Run() = 0;
	};

}