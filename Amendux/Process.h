#pragma once

namespace Amendux {

	class Process
	{

	public:
		Process() {}
		~Process() {}

		static bool UpdateInstance(const std::wstring& file);
		static bool KillProcess(unsigned int pid);
		static bool EndProcess(unsigned int pid, DWORD dwTimeout = 0);
	};

}
