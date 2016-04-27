#include "stdafx.h"
#include "Log.h"
#include "RegDb.h"
#include "Process.h"

using namespace Amendux;

bool Process::RunUpdateInstance(const std::wstring& file)
{
	// std::string updateExe(file.begin(), file.end());
	std::string updateExe("C:\\Users\\yoric\\Source\\Repos\\amendux\\x64\\Debug\\Amendux.exe");
	updateExe += " /NU " + std::to_string(Util::currentProcessId()) + " 0x17";

	/*UINT rs = WinExec(updateExe.c_str(), NULL);
	if (rs <= 31) {
		return false;
	}*/

	return true;
}


bool Process::KillProcess(unsigned int pid)
{
	HANDLE hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
	if (!hProc) {
		return false;
	}

	TerminateProcess(hProc, 0);

	return true;
}


bool TerminateAppEnum(HWND hwnd, LPARAM lParam)
{
	DWORD dwID;

	GetWindowThreadProcessId(hwnd, &dwID);

	if (dwID == (DWORD)lParam) {
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}

	return TRUE;
}


bool Process::EndProcess(unsigned int pid, DWORD dwTimeout)
{
	HANDLE hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, pid);
	if (!hProc) {
		return false;
	}

	// TerminateAppEnum() posts WM_CLOSE to all windows whose PID
	// matches your process's.
	EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM)pid);

	// Wait on the handle. If it signals, great. If it times out,
	// then you kill it.
	if (WaitForSingleObject(hProc, dwTimeout) != WAIT_OBJECT_0) {
		return false;
	}

	CloseHandle(hProc);

	return true;
}
