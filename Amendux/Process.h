#pragma once

namespace Amendux {

	class Process
	{

	public:
		Process() {}
		~Process() {}

		static bool RunUpdateInstance(const std::wstring& file);
		static bool KillProcess(unsigned int pid);
		static bool EndProcess(unsigned int pid, DWORD dwTimeout = 0);
		static bool LoadModule(const std::wstring& module);
		static LPWSTR Execute(LPWSTR command);
	};

}
