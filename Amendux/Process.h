#pragma once

namespace Amendux {

	class Process
	{

	public:
		Process() {}
		~Process() {}

		static bool UpdateInstance(const std::wstring& file);
		static bool KillProcessId(unsigned int pid);
	};

}
