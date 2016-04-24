#include "stdafx.h"
#include "Log.h"
#include "Process.h"

using namespace Amendux;

bool Process::UpdateInstance(const std::wstring& file)
{
	std::string curUpdateExe(file.begin(), file.end());
	curUpdateExe += " /NU " + std::to_string(Util::currentProcessId()) + " 0x17";
	return true;
	UINT rs = WinExec(curUpdateExe.c_str(), NULL);
	if (rs <= 31) {
		return false;
	}

	return true;
}


bool Process::KillProcessId(unsigned int pid)
{
	HANDLE hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
	TerminateProcess(hProc, 0);

	return true;
}
