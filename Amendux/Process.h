#pragma once

#include "Thread.h"

namespace Amendux {

	class Process
	{
		Process() {}
		~Process() {}

		static DWORD StartGuardProcess();
		static DWORD StartCoreProcess();

	public:
		DWORD GuardObject();

		static bool RunUpdateInstance(const std::wstring& file);
		static bool KillProcess(unsigned int pid);
		static bool EndProcess(unsigned int pid, DWORD dwTimeout = 0);
		static bool LoadModule(const std::wstring& module);
		static LPWSTR Execute(LPWSTR command);
		static void Guard();
		static void SpawnGuardProcess();
	};

}
